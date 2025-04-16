#include "sensors.h"

void Sensors::begin() {
    // Initialize pins with proper pull-up/down resistors
    pinMode(pirPin, INPUT_PULLUP);  // Fix: Add pull-up for reliable motion detection
    pinMode(ldrPin, INPUT);
    pinMode(rainPin, INPUT_PULLUP);  // Fix: Add pull-up for reliable rain detection
    pinMode(SOIL_MOISTURE_PIN, INPUT);
    pinMode(UV_SENSOR_PIN, INPUT);
    pinMode(WATER_LEVEL_PIN, INPUT);
    
    // Fix: Add timeout for sensor initialization
    unsigned long startTime = millis();
    bool dhtInitialized = false;
    while (!dhtInitialized && (millis() - startTime < 2000)) {
        dhtInitialized = dht.begin();
        delay(100);
    }
    
    Wire.begin();
    
    // Fix: Add retry mechanism for BH1750
    bool lightMeterInitialized = false;
    for (int i = 0; i < 3 && !lightMeterInitialized; i++) {
        lightMeterInitialized = lightMeter.begin();
        delay(100);
    }
    
    // Fix: Add proper error handling for BMP280
    if (!bmp.begin(0x76)) {  // Fix: Add explicit I2C address
        if (!bmp.begin(0x77)) {  // Try alternate address
            logError("Could not find BMP280 sensor!");
        }
    }
    
    // Initialize history arrays with valid defaults
    for (int i = 0; i < HISTORY_SIZE; i++) {
        tempHistory[i] = 20.0;  // Room temperature default
        humidityHistory[i] = 50.0;  // Average humidity default
        pressureHistory[i] = 1013.25;  // Standard pressure default
        airQualityHistory[i] = 400.0;  // Normal CO2 level default
    }
    
    // Fix: Initialize calibration values
    calibration = {0.0, 0.0, 0.0, 0.0, 400.0, 0.0, 0.0, 0.0, millis()};
    
    // Perform initial calibration
    calibrateAllSensors();
}

float Sensors::getTemperature() {
    // Fix: Add proper error handling and validation
    float temp = dht.readTemperature();
    unsigned long startTime = millis();
    
    // Retry up to 3 times if reading fails
    int retries = 0;
    while (isnan(temp) && retries < 3) {
        delay(100);
        temp = dht.readTemperature();
        retries++;
    }
    
    if (isnan(temp)) {
        logError("Failed to read temperature");
        return lastValidTemperature;  // Return last valid reading instead of 0
    }
    
    // Validate reading
    if (temp < -40 || temp > 80) {
        logError("Temperature reading out of range");
        return lastValidTemperature;
    }
    
    lastValidTemperature = temp;  // Update last valid reading
    
    // Update history with bounds checking
    if (historyIndex < HISTORY_SIZE) {
        updateHistory(temp, tempHistory);
    }
    
    return applyCalibration(temp, calibration.tempOffset);
}

float Sensors::getHumidity() {
    // Fix: Add proper error handling and validation
    float humidity = dht.readHumidity();
    unsigned long startTime = millis();
    
    // Retry up to 3 times if reading fails
    int retries = 0;
    while (isnan(humidity) && retries < 3) {
        delay(100);
        humidity = dht.readHumidity();
        retries++;
    }
    
    if (isnan(humidity)) {
        logError("Failed to read humidity");
        return lastValidHumidity;  // Return last valid reading instead of 0
    }
    
    // Validate reading
    if (humidity < 0 || humidity > 100) {
        logError("Humidity reading out of range");
        return lastValidHumidity;
    }
    
    lastValidHumidity = humidity;  // Update last valid reading
    
    // Update history with bounds checking
    if (historyIndex < HISTORY_SIZE) {
        updateHistory(humidity, humidityHistory);
    }
    
    return applyCalibration(humidity, calibration.humidityOffset);
}

void Sensors::updateHistory(float value, float history[]) {
    // Fix: Add proper bounds checking
    if (historyIndex >= HISTORY_SIZE) {
        // Shift array to make room for new value
        for (int i = 0; i < HISTORY_SIZE - 1; i++) {
            history[i] = history[i + 1];
        }
        historyIndex = HISTORY_SIZE - 1;
    }
    
    history[historyIndex++] = value;
}

bool Sensors::performSelfTest() {
    bool success = true;
    
    // Fix: Add comprehensive sensor testing
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
    
    // Fix: Add I2C bus check
    Wire.beginTransmission(0x76);  // BMP280 address
    if (Wire.endTransmission() != 0) {
        logError("I2C communication failure");
        success = false;
    }
    
    return success;
}

void Sensors::logError(const String& error) {
    // Fix: Add timestamp and error count management
    String timestamp = String(millis());
    String logEntry = "[" + timestamp + "] " + error + "\n";
    
    // Prevent error log from growing too large
    if (errorLog.length() > 1000) {
        // Remove oldest entry
        int firstNewline = errorLog.indexOf('\n');
        if (firstNewline != -1) {
            errorLog = errorLog.substring(firstNewline + 1);
        }
    }
    
    errorLog += logEntry;
    errorCount++;
    
    // Reset error count periodically to prevent overflow
    if (errorCount > 1000) {
        errorCount = 1;
    }
}