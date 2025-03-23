#include "sensors.h"

Sensors::Sensors(uint8_t dhtPin, uint8_t pirPin, uint8_t ldrPin) 
    : dht(dhtPin, DHT22), pirPin(pirPin), ldrPin(ldrPin),
      lastMotionTime(0), motionState(false), historyIndex(0),
      lastUpdate(0), sensorError(false), errorCount(0) {
    
    // Initialize calibration values
    calibration = {0, 0, 0, 0, 0};
    
    // Set default thresholds
    thresholds = {
        15, 35,    // Temperature min/max
        20, 80,    // Humidity min/max
        980, 1020, // Pressure min/max
        0, 1000,   // Light min/max
        30         // Air quality min
    };
    
    updateInterval = 2000; // 2 seconds default
}

float Sensors::getDewPoint() {
    float temp = getTemperature();
    float humidity = getHumidity();
    return calculateDewPoint(temp, humidity);
}

float Sensors::getHeatIndex() {
    return dht.computeHeatIndex(getTemperature(), getHumidity(), false);
}

float Sensors::getUVIndex() {
    // Implementation for UV sensor
    return 0;
}

float Sensors::getVOCLevel() {
    // Implementation for VOC sensor
    return 0;
}

float Sensors::getOzoneLevel() {
    // Implementation for ozone sensor
    return 0;
}

float Sensors::getAirQualityTrend() {
    return calculateTrend(airQualityHistory, HISTORY_SIZE);
}

float Sensors::getSoilMoisture() {
    // Implementation for soil moisture sensor
    return 0;
}

float Sensors::getSoilTemperature() {
    // Implementation for soil temperature sensor
    return 0;
}

float Sensors::getSoilPH() {
    // Implementation for soil pH sensor
    return 0;
}

float Sensors::getSoundLevel() {
    // Implementation for sound sensor
    return 0;
}

float Sensors::getRadiationLevel() {
    // Implementation for radiation sensor
    return 0;
}

float Sensors::getPredictedTemperature(int hoursAhead) {
    // Simple linear regression for prediction
    float trend = getTemperatureTrend();
    return getTemperature() + (trend * hoursAhead);
}

float Sensors::getComfortIndex() {
    float temp = getTemperature();
    float humidity = getHumidity();
    float comfort = 0;
    
    // Calculate comfort index based on temperature and humidity
    if (temp >= 20 && temp <= 25 && humidity >= 30 && humidity <= 60) {
        comfort = 100;
    } else {
        comfort = 100 - (abs(temp - 22.5) * 5) - (abs(humidity - 45) * 0.5);
    }
    
    return constrain(comfort, 0, 100);
}

float Sensors::getAirQualityIndex() {
    float aqi = 0;
    float co2 = getCO2Level();
    float voc = getVOCLevel();
    float ozone = getOzoneLevel();
    
    // Calculate AQI based on multiple factors
    aqi = (co2 / 1000.0 + voc / 100.0 + ozone / 50.0) * 33.33;
    return constrain(aqi, 0, 100);
}

bool Sensors::isPrecipitationLikely() {
    float pressure = getPressure();
    float pressureTrend = getPressureTrend();
    
    return pressureTrend < -0.5 && pressure < 1010;
}

void Sensors::setUpdateInterval(unsigned long interval) {
    updateInterval = interval;
}

bool Sensors::performSelfTest() {
    bool success = true;
    
    // Test temperature sensor
    float temp = getTemperature();
    if (!validateReading(temp, thresholds.tempMin, thresholds.tempMax)) {
        logError("Temperature sensor failure");
        success = false;
    }
    
    // Test humidity sensor
    float humidity = getHumidity();
    if (!validateReading(humidity, thresholds.humidityMin, thresholds.humidityMax)) {
        logError("Humidity sensor failure");
        success = false;
    }
    
    return success;
}

float Sensors::getBatteryLevel() {
    // Implementation for battery monitoring
    return 100;
}

bool Sensors::getSensorStatus(const String& sensorName) {
    // Return status for specific sensor
    return !sensorError;
}

void Sensors::getErrorLog(String& log) {
    log = errorLog;
}

void Sensors::logError(const String& error) {
    errorLog += error + "\n";
    errorCount++;
}

bool Sensors::validateReading(float value, float min, float max) {
    return value >= min && value <= max;
}

void Sensors::updateSensorStatus() {
    // Update overall sensor status
    sensorError = errorCount > 0;
}

float Sensors::applyCalibration(float value, float offset) {
    return value + offset;
}

float Sensors::calculateDewPoint(float temperature, float humidity) {
    float a = 17.271;
    float b = 237.7;
    float temp = (a * temperature) / (b + temperature) + log(humidity/100);
    return (b * temp) / (a - temp);
}