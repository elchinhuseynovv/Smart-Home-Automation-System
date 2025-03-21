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

// Melody for alerts
const int alertMelody[] = {2000, 1500, 2000, 1500};
const int alertDurations[] = {200, 200, 200, 200};

// Weather prediction data
struct WeatherData {
    float temperature;
    float humidity;
    float pressure;
    float rainProbability;
    float uvIndex;
} weatherForecast;

void setup() {
    Serial.begin(9600);
    
    // Initialize components
    sensors.begin();
    if (!display.begin()) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;);
    }
    actuators.begin();
    automation.begin();
    network.begin();
    storage.begin();
    
    // Load saved settings
    loadSystemSettings();
    
    // Initial display message
    display.showAlert("System Starting...");
    delay(2000);
    
    // Initialize weather data
    updateWeatherForecast();
}

void loop() {
    unsigned long currentMillis = millis();
    
    // Read sensors at regular intervals
    if (currentMillis - lastSensorRead >= SENSOR_READ_INTERVAL) {
        lastSensorRead = currentMillis;
        
        // Get comprehensive sensor readings
        SensorData sensorData = {
            .temperature = sensors.getAverageTemperature(),
            .humidity = sensors.getAverageHumidity(),
            .pressure = sensors.getAveragePressure(),
            .motion = sensors.getMotion(),
            .lightLevel = sensors.getPreciseLightLevel(),
            .isRaining = sensors.isRaining(),
            .airQuality = sensors.getAirQuality(),
            .soilMoisture = sensors.getSoilMoisture(),
            .uvIndex = sensors.getUVIndex(),
            .waterLevel = sensors.getWaterLevel()
        };
        
        // Update temperature history
        temperatureHistory[historyIndex] = sensorData.temperature;
        historyIndex = (historyIndex + 1) % 24;
        
        // Automatic control logic
        if (autoMode) {
            // Climate control
            automation.handleClimateControl(sensorData, weatherForecast);
            
            // Garden automation
            if (gardenMode) {
                automation.handleGardenCare(sensorData, weatherForecast);
            }
            
            // Energy management
            if (energySaveMode) {
                automation.handleEnergyManagement(sensorData);
            }
            
            // Security monitoring
            automation.handleSecurity(sensorData);
            
            // Comfort optimization
            automation.optimizeComfort(sensorData);
        }
        
        // Emergency conditions check
        checkEmergencyConditions(sensorData);
        
        // Log data periodically
        if (currentMillis - lastDataLog >= DATA_LOGGING_INTERVAL) {
            lastDataLog = currentMillis;
            storage.logSensorData(sensorData);
        }
        
        // Update weather forecast periodically
        if (currentMillis - lastWeatherUpdate >= WEATHER_UPDATE_INTERVAL) {
            lastWeatherUpdate = currentMillis;
            updateWeatherForecast();
        }
        
        // Update display with current readings
        if (currentMillis - lastDisplayUpdate >= DISPLAY_UPDATE_INTERVAL) {
            lastDisplayUpdate = currentMillis;
            updateDisplayInfo(sensorData);
        }
        
        // Network updates
        network.sendStatusUpdate(sensorData);
        handleNetworkCommands();
    }
    
    // Check for door auto-close
    if (actuators.getDoorState() != LOCKED && 
        currentMillis - lastSensorRead >= AUTO_CLOSE_DELAY) {
        actuators.setDoorState(LOCKED);
        Serial.println("Auto-closing door");
    }
}

void checkEmergencyConditions(const SensorData& data) {
    bool emergency = false;
    String alertMessage = "";
    
    if (data.temperature > 35.0) {
        emergency = true;
        alertMessage += "High Temperature!\n";
    }
    
    if (data.airQuality < 30.0) {
        emergency = true;
        alertMessage += "Poor Air Quality!\n";
    }
    
    if (data.waterLevel < LOW_WATER_THRESHOLD) {
        emergency = true;
        alertMessage += "Low Water Level!\n";
    }
    
    if (data.uvIndex > HIGH_UV_THRESHOLD) {
        emergency = true;
        alertMessage += "High UV Level!\n";
    }
    
    if (emergency) {
        actuators.playMelody(alertMelody, alertDurations, 4);
        display.showAlert(alertMessage.c_str());
        network.sendEmergencyAlert(alertMessage);
        delay(2000);
    }
}

void updateWeatherForecast() {
    WeatherData forecast = network.getWeatherForecast();
    if (forecast.temperature != 0) { // Valid forecast
        weatherForecast = forecast;
        automation.updateWeatherStrategy(forecast);
    }
}

void updateDisplayInfo(const SensorData& data) {
    switch (display.getCurrentPage()) {
        case MAIN:
            display.updateStatus(data.temperature, data.humidity, data.motion,
                               data.lightLevel, data.isRaining, data.airQuality);
            break;
            
        case ENVIRONMENT:
            display.showEnvironmentStatus(data);
            break;
            
        case GARDEN:
            display.showGardenStatus(data.soilMoisture, data.waterLevel, data.uvIndex);
            break;
            
        case WEATHER:
            display.showWeatherInfo(weatherForecast);
            break;
            
        case ENERGY:
            display.showEnergyStatus(automation.getEnergyStats());
            break;
    }
}

void loadSystemSettings() {
    SystemSettings settings = storage.loadSettings();
    autoMode = settings.autoMode;
    energySaveMode = settings.energySaveMode;
    gardenMode = settings.gardenMode;
    automation.setThresholds(settings.thresholds);
}

void handleNetworkCommands() {
    Command cmd = network.getNextCommand();
    if (cmd.type != NONE) {
        automation.handleCommand(cmd);
    }
}