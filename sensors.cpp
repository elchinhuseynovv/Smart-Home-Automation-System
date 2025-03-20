#include "sensors.h"

Sensors::Sensors(uint8_t dhtPin, uint8_t pirPin, uint8_t ldrPin) 
    : dht(dhtPin, DHT22),
      pirPin(pirPin), 
      ldrPin(ldrPin),
      airSensor(A1),
      lastMotionTime(0),
      motionState(false),
      historyIndex(0),
      airQualityBaseline(0),
      pressureBaseline(0),
      seaLevelPressure(1013.25) {
    
    for(int i = 0; i < HISTORY_SIZE; i++) {
        tempHistory[i] = 0;
        humidityHistory[i] = 0;
        pressureHistory[i] = 0;
    }
}

void Sensors::begin() {
    dht.begin();
    pinMode(pirPin, INPUT);
    pinMode(ldrPin, INPUT);
    pinMode(rainPin, INPUT);
    Wire.begin();
    lightMeter.begin();
    bmp.begin(0x76);
    
    // Initial calibration
    calibrateAirSensor();
    calibratePressureSensor();
}

float Sensors::getTemperature() {
    float temp = dht.readTemperature();
    if (!isnan(temp)) {
        updateHistory(temp, tempHistory);
    }
    return isnan(temp) ? -999 : temp;
}

float Sensors::getHumidity() {
    float humidity = dht.readHumidity();
    if (!isnan(humidity)) {
        updateHistory(humidity, humidityHistory);
    }
    return isnan(humidity) ? -999 : humidity;
}

float Sensors::getPressure() {
    float pressure = bmp.readPressure() / 100.0F; // Convert to hPa
    if (pressure > 0) {
        updateHistory(pressure, pressureHistory);
    }
    return pressure;
}

float Sensors::getAltitude() {
    return bmp.readAltitude(seaLevelPressure);
}

bool Sensors::getMotion() {
    bool currentReading = digitalRead(pirPin) == HIGH;
    unsigned long currentTime = millis();
    
    if (currentReading != motionState) {
        if (currentTime - lastMotionTime >= 2000) {
            motionState = currentReading;
            lastMotionTime = currentTime;
        }
    }
    
    return motionState;
}

int Sensors::getLightLevel() {
    return analogRead(ldrPin);
}

float Sensors::getPreciseLightLevel() {
    return lightMeter.readLightLevel();
}

bool Sensors::isRaining() {
    return analogRead(rainPin) < 500;
}

float Sensors::getAirQuality() {
    float reading = airSensor.getPPM();
    return map(reading, airQualityBaseline, airQualityBaseline * 3, 100, 0);
}

float Sensors::getCO2Level() {
    return airSensor.getCO2PPM();
}

float Sensors::getGasLevel() {
    return airSensor.getCorrectedPPM(getTemperature(), getHumidity());
}

float Sensors::getAverageTemperature(int samples) {
    float readings[samples];
    for (int i = 0; i < samples; i++) {
        readings[i] = getTemperature();
        delay(100);
    }
    return calculateAverage(readings, samples);
}

float Sensors::getAverageHumidity(int samples) {
    float readings[samples];
    for (int i = 0; i < samples; i++) {
        readings[i] = getHumidity();
        delay(100);
    }
    return calculateAverage(readings, samples);
}

float Sensors::getAveragePressure(int samples) {
    float readings[samples];
    for (int i = 0; i < samples; i++) {
        readings[i] = getPressure();
        delay(100);
    }
    return calculateAverage(readings, samples);
}

float Sensors::getAverageAirQuality(int samples) {
    float readings[samples];
    for (int i = 0; i < samples; i++) {
        readings[i] = getAirQuality();
        delay(100);
    }
    return calculateAverage(readings, samples);
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

void Sensors::calibrateAirSensor() {
    float sum = 0;
    const int samples = 10;
    
    for(int i = 0; i < samples; i++) {
        sum += airSensor.getRZero();
        delay(100);
    }
    
    airQualityBaseline = sum / samples;
}

void Sensors::calibratePressureSensor() {
    float sum = 0;
    const int samples = 10;
    
    for(int i = 0; i < samples; i++) {
        sum += bmp.readPressure();
        delay(100);
    }
    
    pressureBaseline = sum / (samples * 100.0F); // Convert to hPa
}

float Sensors::calculateAverage(float readings[], int count) {
    float sum = 0;
    int validReadings = 0;
    
    for (int i = 0; i < count; i++) {
        if (readings[i] != -999) {
            sum += readings[i];
            validReadings++;
        }
    }
    
    return validReadings > 0 ? sum / validReadings : -999;
}

float Sensors::calculateTrend(float history[], int count) {
    if (count < 2) return 0;
    
    float sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0;
    int n = count;
    
    for (int i = 0; i < n; i++) {
        sumX += i;
        sumY += history[i];
        sumXY += i * history[i];
        sumX2 += i * i;
    }
    
    float slope = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
    return slope;
}

void Sensors::updateHistory(float value, float history[]) {
    history[historyIndex] = value;
    historyIndex = (historyIndex + 1) % HISTORY_SIZE;
}