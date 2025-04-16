#include "actuators.h"

void Actuators::begin() {
    // Fix: Add proper pin mode initialization
    pinMode(fanPin, OUTPUT);
    pinMode(buzzerPin, OUTPUT);
    
    // Fix: Add servo initialization with error checking
    bool doorServoInitialized = false;
    bool windowServoInitialized = false;
    
    for (int i = 0; i < 3 && !doorServoInitialized; i++) {
        doorServoInitialized = doorServo.attach(servoPin);
        delay(100);
    }
    
    for (int i = 0; i < 3 && !windowServoInitialized; i++) {
        windowServoInitialized = windowServo.attach(windowServoPin);
        delay(100);
    }
    
    if (!doorServoInitialized || !windowServoInitialized) {
        // Handle servo initialization failure
        systemActive = false;
        return;
    }
    
    // Fix: Initialize LED strip with error checking
    leds = new CRGB[30];
    if (!leds) {
        systemActive = false;
        return;
    }
    
    // Fix: Add proper FastLED initialization
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, 30);
    FastLED.setCorrection(TypicalLEDStrip);
    FastLED.setTemperature(DirectSunlight);
    FastLED.setMaxPowerInVoltsAndMilliamps(5, 500);  // 5V, 500mA
    
    // Set initial states
    setDoorState(LOCKED);
    setWindowOpening(0);
    FastLED.clear();
    FastLED.show();
    
    systemActive = true;
}

void Actuators::setDoorState(DoorState state) {
    // Fix: Add proper validation and error handling
    if (!systemActive) return;
    
    if (vacationMode && state != LOCKED) {
        // Don't allow unlocking during vacation mode
        return;
    }
    
    // Fix: Add proper servo angle validation
    int angle;
    switch (state) {
        case LOCKED:
            angle = 0;
            break;
        case UNLOCKED:
            angle = 90;
            break;
        case PARTIALLY_OPEN:
            angle = 45;
            break;
        default:
            return;  // Invalid state
    }
    
    // Fix: Add smooth servo movement
    int currentAngle = doorServo.read();
    int step = (angle > currentAngle) ? 1 : -1;
    
    for (int i = currentAngle; i != angle; i += step) {
        doorServo.write(i);
        delay(15);  // Smooth movement
    }
    
    currentDoorState = state;
    lastDoorOperation = millis();
}

void Actuators::setFan(FanSpeed speed) {
    // Fix: Add proper validation and ramping
    if (!systemActive || nightMode) return;
    
    // Gradually change fan speed
    int currentSpeed = analogRead(fanPin) / 4;  // Convert 10-bit to 8-bit
    int targetSpeed = speed;
    
    // Ramp up/down gradually
    while (currentSpeed != targetSpeed) {
        if (currentSpeed < targetSpeed) {
            currentSpeed++;
        } else {
            currentSpeed--;
        }
        analogWrite(fanPin, currentSpeed);
        delay(10);  // Smooth transition
    }
    
    currentFanSpeed = speed;
}