#include "sensors.h"

void Sensors::begin() {
    pinMode(pirPin, INPUT);
    pinMode(ldrPin, INPUT);
    pinMode(rainPin, INPUT);
    pinMode(SOIL_MOISTURE_PIN, INPUT);
    pinMode(UV_SENSOR_PIN, INPUT);
    pinMode(WATER_LEVEL_PIN, INPUT);
    
    dht.begin();
    Wire.begin();
    lightMeter.begin();
    
    if (!bmp.begin()) {
        logError("Could not find BMP280 sensor!");
    }
    
    // Initialize history arrays
    for (int i = 0; i < HISTORY_SIZE; i++) {
        tempHistory[i] = 0;
        humidityHistory[i] = 0;
        pressureHistory[i] = 0;
        airQualityHistory[i] = 0;
    }
    
    // Initial calibration
    calibrateAllSensors();
}

void Sensors::calibrateAllSensors() {
    calibrateAirSensor();
    calibratePressureSensor();
    calibrateLightSensor();
    calibrateUVSensor();
}

void Sensors::calibrateLightSensor() {
    float sum = 0;
    const int samples = 10;
    
    for (int i = 0; i < samples; i++) {
        sum += lightMeter.readLightLevel();
        delay(100);
    }
    
    calibration.lightOffset = 1000 - (sum / samples); // Calibrate to 1000 lux reference
}

void Sensors::calibrateUVSensor() {
    float sum = 0;
    const int samples = 10;
    
    for (int i = 0; i < samples; i++) {
        sum += analogRead(UV_SENSOR_PIN);
        delay(100);
    }
    
    float avgUV = sum / samples;
    calibration.uvOffset = map(avgUV, 0, 1023, 0, 15); // Map to UV index scale
}

float Sensors::getTemperature() {
    float temp = dht.readTemperature();
    if (isnan(temp)) {
        logError("Failed to read temperature");
        return 0;
    }
    
    // Update history
    updateHistory(temp, tempHistory);
    
    return applyCalibration(temp, calibration.tempOffset);
}

float Sensors::getHumidity() {
    float humidity = dht.readHumidity();
    if (isnan(humidity)) {
        logError("Failed to read humidity");
        return 0;
    }
    
    // Update history
    updateHistory(humidity, humidityHistory);
    
    return applyCalibration(humidity, calibration.humidityOffset);
}

float Sensors::getPressure() {
    float pressure = bmp.readPressure() / 100.0F; // Convert to hPa
    
    // Update history
    updateHistory(pressure, pressureHistory);
    
    return applyCalibration(pressure, calibration.pressureOffset);
}

float Sensors::getAltitude() {
    return bmp.readAltitude(1013.25); // Standard pressure level
}

bool Sensors::getMotion() {
    bool currentMotion = digitalRead(pirPin) == HIGH;
    unsigned long currentTime = millis();
    
    if (currentMotion && !motionState) {
        motionState = true;
        lastMotionTime = currentTime;
    } else if (motionState && (currentTime - lastMotionTime > 30000)) { // 30 second timeout
        motionState = false;
    }
    
    return motionState;
}

int Sensors::getLightLevel() {
    return analogRead(ldrPin);
}

float Sensors::getPreciseLightLevel() {
    float lux = lightMeter.readLightLevel();
    return applyCalibration(lux, calibration.lightOffset);
}

bool Sensors::isRaining() {
    int rainValue = analogRead(rainPin);
    return rainValue < 500; // Adjust threshold as needed
}

float Sensors::getAirQuality() {
    float reading = airSensor.getCorrectedPPM(getTemperature(), getHumidity());
    
    // Update history
    updateHistory(reading, airQualityHistory);
    
    return applyCalibration(reading, calibration.airQualityBaseline);
}

float Sensors::getCO2Level() {
    return airSensor.getPPM();
}

float Sensors::getGasLevel() {
    return analogRead(A0); // Assuming MQ135 is connected to A0
}

float Sensors::getUVIndex() {
    float uvRaw = analogRead(UV_SENSOR_PIN);
    float uvIndex = map(uvRaw, 0, 1023, 0, 15); // Map to UV index scale
    return applyCalibration(uvIndex, calibration.uvOffset);
}

float Sensors::getSoilMoisture() {
    float moisture = analogRead(SOIL_MOISTURE_PIN);
    return map(moisture, 0, 1023, 0, 100); // Convert to percentage
}

float Sensors::getWaterLevel() {
    float level = analogRead(WATER_LEVEL_PIN);
    return map(level, 0, 1023, 0, 100); // Convert to percentage
}

float Sensors::getAverageTemperature(int samples) {
    if (samples <= 0) return getTemperature();
    
    float sum = 0;
    for (int i = 0; i < samples; i++) {
        sum += getTemperature();
        delay(100);
    }
    return sum / samples;
}

float Sensors::getAverageHumidity(int samples) {
    if (samples <= 0) return getHumidity();
    
    float sum = 0;
    for (int i = 0; i < samples; i++) {
        sum += getHumidity();
        delay(100);
    }
    return sum / samples;
}

float Sensors::getAveragePressure(int samples) {
    if (samples <= 0) return getPressure();
    
    float sum = 0;
    for (int i = 0; i < samples; i++) {
        sum += getPressure();
        delay(100);
    }
    return sum / samples;
}

float Sensors::getTemperatureTrend() {
    return calculateTrend(tempHistory, HISTORY_SIZE);
}

float Sensors::getHumidityTrend() {
    return calculateTrend(humidityHistory, HISTORY_SIZE);
}

float Sensors::getPressureTrend() {
    return calculateTrend(pressureHistory, HISTORY_SIZE);
}

float Sensors::calculateTrend(float history[], int count) {
    if (count < 2) return 0;
    
    float sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0;
    for (int i = 0; i < count; i++) {
        sumX += i;
        sumY += history[i];
        sumXY += i * history[i];
        sumX2 += i * i;
    }
    
    float n = count;
    float slope = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
    return slope;
}

void Sensors::updateHistory(float value, float history[]) {
    if (historyIndex >= HISTORY_SIZE) {
        historyIndex = 0;
    }
    
    history[historyIndex++] = value;
}

void Sensors::calibrateAirSensor() {
    float sum = 0;
    const int samples = 10;
    
    for (int i = 0; i < samples; i++) {
        sum += airSensor.getCorrectedPPM(getTemperature(), getHumidity());
        delay(1000);
    }
    
    calibration.airQualityBaseline = sum / samples;
}

void Sensors::calibratePressureSensor() {
    float sum = 0;
    const int samples = 10;
    
    for (int i = 0; i < samples; i++) {
        sum += bmp.readPressure();
        delay(100);
    }
    
    float avgPressure = sum / (samples * 100.0F); // Convert to hPa
    calibration.pressureOffset = 1013.25 - avgPressure; // Calibrate to standard pressure
}

bool Sensors::performSelfTest() {
    bool success = true;
    
    // Test temperature sensor
    float temp = getTemperature();
    if (isnan(temp) || temp < -40 || temp > 80) {
        logError("Temperature sensor failure");
        success = false;
    }
    
    // Test humidity sensor
    float humidity = getHumidity();
    if (isnan(humidity) || humidity < 0 || humidity > 100) {
        logError("Humidity sensor failure");
        success = false;
    }
    
    // Test pressure sensor
    float pressure = getPressure();
    if (isnan(pressure) || pressure < 800 || pressure > 1200) {
        logError("Pressure sensor failure");
        success = false;
    }
    
    // Test light sensor
    float light = getPreciseLightLevel();
    if (isnan(light) || light < 0) {
        logError("Light sensor failure");
        success = false;
    }
    
    return success;
}

void Sensors::logError(const String& error) {
    errorLog += "[" + String(millis()) + "] " + error + "\n";
    errorCount++;
    
    // Prevent error log from growing too large
    if (errorLog.length() > 1000) {
        int firstNewline = errorLog.indexOf('\n');
        if (firstNewline != -1) {
            errorLog = errorLog.substring(firstNewline + 1);
        }
    }
}

float Sensors::applyCalibration(float value, float offset) {
    return value + offset;
}