#include "sensors.h"
#include "display.h"
#include "actuators.h"
#include "automation.h"
#include "network.h"
#include "storage.h"

// Pin Definitions
#define DHTPIN 2
#define PIRPIN 3
#define LEDPIN 4
#define FANPIN 5
#define SERVO_PIN 6
#define WINDOW_SERVO_PIN 7
#define LDRPIN A0
#define BUZZERPIN 8
#define RAIN_SENSOR_PIN A2
#define SOIL_MOISTURE_PIN A3
#define UV_SENSOR_PIN A4
#define WATER_LEVEL_PIN A5

// Create objects
Sensors sensors(DHTPIN, PIRPIN, LDRPIN);
Display display;
Actuators actuators(LEDPIN, FANPIN, BUZZERPIN, SERVO_PIN, WINDOW_SERVO_PIN);
Automation automation;
NetworkManager network;
Storage storage;

// Constants and thresholds
const float TEMP_THRESHOLD = 28.0;
const float HUMIDITY_THRESHOLD = 65.0;
const int LIGHT_THRESHOLD = 300;
const float COMFORT_TEMP_MIN = 20.0;
const float COMFORT_TEMP_MAX = 26.0;
const float HIGH_TEMP_THRESHOLD = 30.0;
const float LOW_MOISTURE_THRESHOLD = 30.0;
const float HIGH_UV_THRESHOLD = 8.0;
const float LOW_WATER_THRESHOLD = 20.0;

// Time intervals
const unsigned long SENSOR_READ_INTERVAL = 2000;
const unsigned long DISPLAY_UPDATE_INTERVAL = 1000;
const unsigned long AUTO_CLOSE_DELAY = 30000;
const unsigned long DATA_LOGGING_INTERVAL = 300000; // 5 minutes
const unsigned long WEATHER_UPDATE_INTERVAL = 1800000; // 30 minutes

// System state
bool autoMode = true;
bool energySaveMode = false;
bool gardenMode = false;
unsigned long lastSensorRead = 0;
unsigned long lastDisplayUpdate = 0;
unsigned long lastDataLog = 0;
unsigned long lastWeatherUpdate = 0;
float temperatureHistory[24] = {0};
int historyIndex = 0;

// Error handling
bool systemError = false;
String errorMessage = "";

void setup() {
    Serial.begin(9600);
    
    // Initialize components with error checking
    sensors.begin();
    if (!sensors.performSelfTest()) {
        systemError = true;
        errorMessage = "Sensor initialization failed";
    }
    
    if (!display.begin()) {
        systemError = true;
        errorMessage += " Display initialization failed";
    }
    
    actuators.begin();
    automation.begin();
    
    if (!network.begin()) {
        Serial.println("Network initialization failed - continuing without network");
    }
    
    if (!storage.begin()) {
        Serial.println("Storage initialization failed - continuing without storage");
    }
    
    // Load saved settings
    loadSystemSettings();
    
    // Initial display message
    if (systemError) {
        display.showAlert("System Error: " + errorMessage);
        delay(5000);
    } else {
        display.showAlert("System Starting...");
        delay(2000);
    }
    
    // Initialize weather data
    updateWeatherForecast();
}

void loop() {
    unsigned long currentMillis = millis();
    
    // Basic error recovery
    if (systemError) {
        handleSystemError();
        return;
    }
    
    // Read sensors at regular intervals with overflow protection
    if (currentMillis - lastSensorRead >= SENSOR_READ_INTERVAL || currentMillis < lastSensorRead) {
        lastSensorRead = currentMillis;
        
        // Get comprehensive sensor readings with error checking
        SensorData sensorData;
        if (!getSensorReadings(&sensorData)) {
            systemError = true;
            errorMessage = "Failed to read sensors";
            return;
        }
        
        // Update temperature history with bounds checking
        if (historyIndex < 24) {
            temperatureHistory[historyIndex] = sensorData.temperature;
            historyIndex = (historyIndex + 1) % 24;
        }
        
        // Automatic control logic with error handling
        if (autoMode) {
            try {
                automation.handleClimateControl(sensorData, weatherForecast);
                
                if (gardenMode) {
                    automation.handleGardenCare(sensorData, weatherForecast);
                }
                
                if (energySaveMode) {
                    automation.handleEnergyManagement(sensorData);
                }
                
                automation.handleSecurity(sensorData);
                automation.optimizeComfort(sensorData);
            } catch (...) {
                systemError = true;
                errorMessage = "Automation error";
                return;
            }
        }
        
        // Emergency conditions check
        checkEmergencyConditions(sensorData);
        
        // Log data periodically with overflow protection
        if (currentMillis - lastDataLog >= DATA_LOGGING_INTERVAL || currentMillis < lastDataLog) {
            lastDataLog = currentMillis;
            if (!storage.logSensorData(sensorData)) {
                Serial.println("Failed to log sensor data");
            }
        }
        
        // Update weather forecast periodically with overflow protection
        if (currentMillis - lastWeatherUpdate >= WEATHER_UPDATE_INTERVAL || currentMillis < lastWeatherUpdate) {
            lastWeatherUpdate = currentMillis;
            updateWeatherForecast();
        }
        
        // Update display with current readings
        if (currentMillis - lastDisplayUpdate >= DISPLAY_UPDATE_INTERVAL || currentMillis < lastDisplayUpdate) {
            lastDisplayUpdate = currentMillis;
            updateDisplayInfo(sensorData);
        }
        
        // Network updates with error handling
        try {
            network.sendStatusUpdate(sensorData);
            handleNetworkCommands();
        } catch (...) {
            Serial.println("Network communication error");
        }
    }
    
    // Check for door auto-close with overflow protection
    if (actuators.getDoorState() != LOCKED && 
        (currentMillis - lastSensorRead >= AUTO_CLOSE_DELAY || currentMillis < lastSensorRead)) {
        actuators.setDoorState(LOCKED);
        Serial.println("Auto-closing door");
    }
}

bool getSensorReadings(SensorData* data) {
    try {
        data->temperature = sensors.getAverageTemperature();
        data->humidity = sensors.getAverageHumidity();
        data->pressure = sensors.getAveragePressure();
        data->motion = sensors.getMotion();
        data->lightLevel = sensors.getPreciseLightLevel();
        data->isRaining = sensors.isRaining();
        data->airQuality = sensors.getAirQuality();
        data->soilMoisture = sensors.getSoilMoisture();
        data->uvIndex = sensors.getUVIndex();
        data->waterLevel = sensors.getWaterLevel();
        return true;
    } catch (...) {
        return false;
    }
}

void handleSystemError() {
    static unsigned long lastErrorDisplay = 0;
    unsigned long currentMillis = millis();
    
    // Display error message every 5 seconds
    if (currentMillis - lastErrorDisplay >= 5000 || currentMillis < lastErrorDisplay) {
        display.showAlert("System Error: " + errorMessage);
        lastErrorDisplay = currentMillis;
    }
    
    // Try to recover
    if (sensors.performSelfTest()) {
        systemError = false;
        errorMessage = "";
        display.showAlert("System recovered");
        delay(2000);
    }
}