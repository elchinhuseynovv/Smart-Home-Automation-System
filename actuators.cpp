#include "actuators.h"

Actuators::Actuators(uint8_t ledPin, uint8_t fanPin, uint8_t buzzerPin, uint8_t servoPin, uint8_t windowServoPin)
    : ledPin(ledPin), fanPin(fanPin), buzzerPin(buzzerPin),
      currentLightLevel(0), currentFanSpeed(OFF), currentDoorState(LOCKED),
      currentWindowOpening(0), systemActive(true), autoFanMode(false),
      nightMode(false), vacationMode(false),
      fanTempThreshold(25.0), lastDoorOperation(0), autoCloseTime(0) {
    
    doorServo.attach(servoPin);
    windowServo.attach(windowServoPin);
    
    // Initialize LED strip
    leds = new CRGB[30]; // Assuming 30 LEDs
    FastLED.addLeds<WS2812B, ledPin, GRB>(leds, 30);
    
    // Initialize schedules
    fanSchedule = {0, 0, false, OFF};
    windowSchedule = {0, 0, false, 0};
    doorSchedule = {0, 0, false, LOCKED};
    
    // Initialize light show
    lightShow = {false, 0, 0, 0};
}

void Actuators::begin() {
    pinMode(fanPin, OUTPUT);
    pinMode(buzzerPin, OUTPUT);
    setDoorState(LOCKED);
    setWindowOpening(0);
    FastLED.clear();
    FastLED.show();
}

// Enhanced lighting control methods
void Actuators::setLight(int brightness) {
    brightness = constrain(brightness, 0, 255);
    fill_solid(leds, 30, CHSV(0, 0, brightness));
    FastLED.show();
    currentLightLevel = brightness;
}

void Actuators::fadeLight(int targetBrightness, int duration) {
    targetBrightness = constrain(targetBrightness, 0, 255);
    int steps = abs(targetBrightness - currentLightLevel);
    int delayTime = duration / steps;
    
    if (targetBrightness > currentLightLevel) {
        for (int i = currentLightLevel; i <= targetBrightness; i++) {
            fill_solid(leds, 30, CHSV(0, 0, i));
            FastLED.show();
            delay(delayTime);
        }
    } else {
        for (int i = currentLightLevel; i >= targetBrightness; i--) {
            fill_solid(leds, 30, CHSV(0, 0, i));
            FastLED.show();
            delay(delayTime);
        }
    }
    currentLightLevel = targetBrightness;
}

void Actuators::setLightMode(LightMode mode) {
    switch (mode) {
        case NORMAL:
            setLight(currentLightLevel);
            break;
        case AMBIENT:
            // Soft, warm white light
            fill_solid(leds, 30, CRGB(255, 200, 100));
            break;
        case NIGHT:
            // Very dim, warm light
            fill_solid(leds, 30, CRGB(50, 30, 10));
            break;
        case PARTY:
            startLightShow(30000); // 30-second light show
            break;
        case ALERT:
            // Red pulsing
            for (int i = 0; i < 3; i++) {
                fill_solid(leds, 30, CRGB::Red);
                FastLED.show();
                delay(500);
                FastLED.clear();
                FastLED.show();
                delay(500);
            }
            break;
    }
    FastLED.show();
}

void Actuators::setAmbientColor(uint8_t r, uint8_t g, uint8_t b) {
    fill_solid(leds, 30, CRGB(r, g, b));
    FastLED.show();
}

void Actuators::startLightShow(int duration) {
    lightShow.active = true;
    lightShow.startTime = millis();
    lightShow.duration = duration;
    lightShow.currentPattern = 0;
}

// Advanced fan control methods
void Actuators::setFan(FanSpeed speed) {
    if (!systemActive || nightMode) return;
    analogWrite(fanPin, speed);
    currentFanSpeed = speed;
}

void Actuators::setFanSchedule(int startHour, int endHour, FanSpeed speed) {
    fanSchedule.startHour = startHour;
    fanSchedule.endHour = endHour;
    fanSchedule.fanSpeed = speed;
    fanSchedule.enabled = true;
}

void Actuators::updateFanControl(float temperature, float humidity) {
    if (!autoFanMode || !systemActive) return;
    
    float heatIndex = dht.computeHeatIndex(temperature, humidity, false);
    
    if (heatIndex > fanTempThreshold + 5) {
        setFan(HIGH);
    } else if (heatIndex > fanTempThreshold + 2) {
        setFan(MEDIUM);
    } else if (heatIndex > fanTempThreshold) {
        setFan(LOW);
    } else {
        setFan(OFF);
    }
}

// Door control methods
void Actuators::setDoorState(DoorState state) {
    if (!systemActive || (vacationMode && state != LOCKED)) return;
    
    switch (state) {
        case LOCKED:
            doorServo.write(0);
            break;
        case UNLOCKED:
            doorServo.write(90);
            break;
        case PARTIALLY_OPEN:
            doorServo.write(45);
            break;
    }
    
    currentDoorState = state;
    lastDoorOperation = millis();
}

void Actuators::setDoorSchedule(int openHour, int closeHour) {
    doorSchedule.startHour = openHour;
    doorSchedule.endHour = closeHour;
    doorSchedule.doorState = UNLOCKED;
    doorSchedule.enabled = true;
}

// Window control methods
void Actuators::updateWindowControl(float temperature, bool isRaining) {
    if (!systemActive || nightMode) return;
    
    if (isRaining) {
        setWindowOpening(0);
        return;
    }
    
    if (temperature > 28) {
        setWindowOpening(100);
    } else if (temperature < 20) {
        setWindowOpening(0);
    } else {
        setWindowOpening(50);
    }
}

void Actuators::setWindowSchedule(int openHour, int closeHour) {
    windowSchedule.startHour = openHour;
    windowSchedule.endHour = closeHour;
    windowSchedule.windowOpening = 50;
    windowSchedule.enabled = true;
}

// System control methods
void Actuators::setNightMode(bool enabled) {
    nightMode = enabled;
    if (enabled) {
        setLight(20);
        setFan(LOW);
        setWindowOpening(20);
    }
}

void Actuators::setVacationMode(bool enabled) {
    vacationMode = enabled;
    if (enabled) {
        setDoorState(LOCKED);
        setWindowOpening(0);
        setFan(OFF);
    }
}

// Private helper methods
void Actuators::updateLightShow() {
    if (!lightShow.active) return;
    
    unsigned long currentTime = millis();
    if (currentTime - lightShow.startTime >= lightShow.duration) {
        lightShow.active = false;
        setLight(currentLightLevel);
        return;
    }
    
    // Create different patterns
    switch (lightShow.currentPattern) {
        case 0: // Rainbow
            fill_rainbow(leds, 30, (currentTime / 50) % 255, 7);
            break;
        case 1: // Theater chase
            for (int i = 0; i < 30; i++) {
                leds[i] = ((i + (currentTime / 200)) % 3 == 0) ? CRGB::White : CRGB::Black;
            }
            break;
        case 2: // Color wipe
            for (int i = 0; i < 30; i++) {
                leds[i] = CHSV((currentTime / 20 + i * 255 / 30) % 255, 255, 255);
            }
            break;
    }
    
    if (currentTime % 5000 == 0) {
        lightShow.currentPattern = (lightShow.currentPattern + 1) % 3;
    }
    
    FastLED.show();
}

void Actuators::handleSchedules() {
    // Get current hour
    int currentHour = (millis() / 3600000) % 24; // Simplified for demonstration
    
    // Handle fan schedule
    if (fanSchedule.enabled) {
        if (currentHour >= fanSchedule.startHour && currentHour < fanSchedule.endHour) {
            setFan(fanSchedule.fanSpeed);
        } else {
            setFan(OFF);
        }
    }
    
    // Handle window schedule
    if (windowSchedule.enabled) {
        if (currentHour >= windowSchedule.startHour && currentHour < windowSchedule.endHour) {
            setWindowOpening(windowSchedule.windowOpening);
        } else {
            setWindowOpening(0);
        }
    }
    
    // Handle door schedule
    if (doorSchedule.enabled) {
        if (currentHour >= doorSchedule.startHour && currentHour < doorSchedule.endHour) {
            setDoorState(doorSchedule.doorState);
        } else {
            setDoorState(LOCKED);
        }
    }
}