#include "sensors.h"

void Sensors::begin() {
    pinMode(pirPin, INPUT);
    pinMode(ldrPin, INPUT);
    dht.begin();
    Wire.begin();
    lightMeter.begin();
    if (!bmp.begin()) {
        logError("Could not find BMP280 sensor!");
    }
}

float Sensors::getTemperature() {
    float temp = dht.readTemperature();
    if (isnan(temp)) {
        logError("Failed to read temperature");
        return 0;
    }
    return applyCalibration(temp, calibration.tempOffset);
}

float Sensors::getHumidity() {
    float humidity = dht.readHumidity();
    if (isnan(humidity)) {
        logError("Failed to read humidity");
        return 0;
    }
    return applyCalibration(humidity, calibration.humidityOffset);
}

float Sensors::getPressure() {
    float pressure = bmp.readPressure() / 100.0F; // Convert to hPa
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
    return analogRead(rainPin) < 500; // Adjust threshold as needed
}

float Sensors::getAirQuality() {
    float reading = airSensor.getCorrectedPPM(getTemperature(), getHumidity());
    return applyCalibration(reading, calibration.airQualityBaseline);
}

float Sensors::getCO2Level() {
    return airSensor.getPPM();
}

float Sensors::getGasLevel() {
    return analogRead(A0); // Assuming MQ135 is connected to A0
}

float Sensors::getAverageTemperature(int samples) {
    float sum = 0;
    for (int i = 0; i < samples; i++) {
        sum += getTemperature();
        delay(100);
    }
    return sum / samples;
}

float Sensors::getAverageHumidity(int samples) {
    float sum = 0;
    for (int i = 0; i < samples; i++) {
        sum += getHumidity();
        delay(100);
    }
    return sum / samples;
}

float Sensors::getAveragePressure(int samples) {
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