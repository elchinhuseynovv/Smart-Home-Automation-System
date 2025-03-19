#include "sensors.h"
#include "display.h"
#include "actuators.h"

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

// Create objects
Sensors sensors(DHTPIN, PIRPIN, LDRPIN);
Display display;
Actuators actuators(LEDPIN, FANPIN, BUZZERPIN, SERVO_PIN, WINDOW_SERVO_PIN);

// Constants and thresholds
const float TEMP_THRESHOLD = 28.0;
const float HUMIDITY_THRESHOLD = 65.0;
const int LIGHT_THRESHOLD = 300;
const float COMFORT_TEMP_MIN = 20.0;
const float COMFORT_TEMP_MAX = 26.0;
const float HIGH_TEMP_THRESHOLD = 30.0;

// Time intervals
const unsigned long SENSOR_READ_INTERVAL = 2000;
const unsigned long DISPLAY_UPDATE_INTERVAL = 1000;
const unsigned long AUTO_CLOSE_DELAY = 30000;

// System state
bool autoMode = true;
unsigned long lastSensorRead = 0;
unsigned long lastDisplayUpdate = 0;
float temperatureHistory[24] = {0};
int historyIndex = 0;

// Melody for alerts
const int alertMelody[] = {2000, 1500, 2000, 1500};
const int alertDurations[] = {200, 200, 200, 200};

void setup() {
    Serial.begin(9600);
    
    // Initialize components
    sensors.begin();
    if (!display.begin()) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;);
    }
    actuators.begin();
    
    // Initial display message
    display.showAlert("System Starting...");
    delay(2000);
}

void loop() {
    unsigned long currentMillis = millis();
    
    // Read sensors at regular intervals
    if (currentMillis - lastSensorRead >= SENSOR_READ_INTERVAL) {
        lastSensorRead = currentMillis;
        
        // Get sensor readings with averaging for stability
        float temperature = sensors.getAverageTemperature();
        float humidity = sensors.getAverageHumidity();
        bool motion = sensors.getMotion();
        float lightLevel = sensors.getPreciseLightLevel();
        bool isRaining = sensors.isRaining();
        float airQuality = sensors.getAirQuality();
        
        // Update temperature history
        temperatureHistory[historyIndex] = temperature;
        historyIndex = (historyIndex + 1) % 24;
        
        // Automatic control logic
        if (autoMode) {
            // Temperature-based climate control
            if (temperature > HIGH_TEMP_THRESHOLD) {
                actuators.setFan(HIGH);
                actuators.setWindowOpening(100);
            } else if (temperature > COMFORT_TEMP_MAX) {
                actuators.setFan(MEDIUM);
                actuators.setWindowOpening(50);
            } else if (temperature < COMFORT_TEMP_MIN) {
                actuators.setFan(OFF);
                actuators.setWindowOpening(0);
            }
            
            // Rain protection
            if (isRaining && actuators.getWindowOpening() > 0) {
                actuators.setWindowOpening(0);
                display.showAlert("Rain detected!\nClosing windows...");
                delay(2000);
            }
            
            // Adaptive lighting based on motion and ambient light
            if (motion) {
                float targetBrightness = map(lightLevel, 0, 1000, 255, 50);
                actuators.fadeLight(targetBrightness, 1000);
            } else {
                actuators.fadeLight(0, 2000);
            }
        }
        
        // Emergency conditions
        if (temperature > 35.0 || airQuality < 30.0) {
            actuators.playMelody(alertMelody, alertDurations, 4);
            display.showAlert("Environmental Alert!");
            delay(2000);
        }
        
        // Update display with current readings
        if (currentMillis - lastDisplayUpdate >= DISPLAY_UPDATE_INTERVAL) {
            lastDisplayUpdate = currentMillis;
            display.updateStatus(temperature, humidity, motion, lightLevel, isRaining, airQuality);
        }
        
        // Log data to Serial
        Serial.println("Status Update:");
        Serial.print("Temperature: "); Serial.println(temperature);
        Serial.print("Humidity: "); Serial.println(humidity);
        Serial.print("Motion: "); Serial.println(motion ? "Yes" : "No");
        Serial.print("Light Level: "); Serial.println(lightLevel);
        Serial.print("Rain: "); Serial.println(isRaining ? "Yes" : "No");
        Serial.print("Air Quality: "); Serial.println(airQuality);
        Serial.println("------------------------");
    }
    
    // Check for door auto-close
    if (actuators.getDoorState() != LOCKED && 
        currentMillis - lastSensorRead >= AUTO_CLOSE_DELAY) {
        actuators.setDoorState(LOCKED);
        Serial.println("Auto-closing door");
    }
}