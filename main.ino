#include "sensors.h"
#include "display.h"
#include "actuators.h"

// Pin Definitions
#define DHTPIN 2
#define PIRPIN 3
#define LEDPIN 4
#define FANPIN 5
#define SERVO_PIN 6
#define LDRPIN A0
#define BUZZERPIN 7

// Create objects
Sensors sensors(DHTPIN, PIRPIN, LDRPIN);
Display display;
Actuators actuators(LEDPIN, FANPIN, BUZZERPIN, SERVO_PIN);

// Constants
const float TEMP_THRESHOLD = 30.0;
const float HUMIDITY_THRESHOLD = 70.0;
const int LIGHT_THRESHOLD = 300;
const float DOOR_TEMP_THRESHOLD = 28.0;
const unsigned long DOOR_OPEN_DURATION = 5000;
const unsigned long ALERT_DURATION = 1000;

void setup() {
    Serial.begin(9600);
    
    // Initialize components
    sensors.begin();
    if (!display.begin()) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;);
    }
    actuators.begin();
}

void loop() {
    // Read sensor values
    float temperature = sensors.getTemperature();
    float humidity = sensors.getHumidity();
    bool motion = sensors.getMotion();
    int lightLevel = sensors.getLightLevel();

    // Motion-activated lights with brightness control
    if (motion) {
        int brightness = map(lightLevel, 0, 1023, 255, 0);
        actuators.setLight(brightness);
        Serial.println("Motion detected: Lights ON");
    } else {
        actuators.setLight(0);
    }

    // Temperature-controlled fan
    if (temperature > TEMP_THRESHOLD) {
        actuators.setFan(true);
        Serial.println("Fan ON: High temperature detected");
    } else {
        actuators.setFan(false);
    }

    // Humidity monitoring
    if (humidity > HUMIDITY_THRESHOLD) {
        Serial.println("Warning: High humidity detected!");
    }

    // Night-time motion alert
    if (motion && lightLevel < LIGHT_THRESHOLD) {
        Serial.println("ALERT: Motion detected at night!");
        actuators.triggerBuzzer(ALERT_DURATION);
    }

    // Update display
    display.updateStatus(temperature, humidity, motion, lightLevel);

    // Smart Door Control
    if (motion && temperature < DOOR_TEMP_THRESHOLD) {
        Serial.println("Unlocking door...");
        actuators.unlockDoor();
        delay(DOOR_OPEN_DURATION);
        actuators.lockDoor();
        Serial.println("Door locked");
    }

    delay(2000);
}