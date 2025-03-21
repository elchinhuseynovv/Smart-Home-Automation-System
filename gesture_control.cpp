#include "gesture_control.h"

GestureControl::GestureControl(uint8_t sensorPin)
    : sensorPin(sensorPin), sensitivity(1.0), isCalibrated(false),
      gestureIndex(0), lastGestureTime(0) {
}

void GestureControl::begin() {
    pinMode(sensorPin, INPUT);
    calibrateSensor();
}

void GestureControl::updateGestures() {
    if (!isCalibrated) return;
    
    GestureType gesture = detectGesture();
    if (gesture != NONE_GESTURE) {
        handleGesture(gesture);
    }
}

GestureType GestureControl::detectGesture() {
    // Read sensor data
    float x = analogRead(sensorPin);
    float y = analogRead(sensorPin + 1);
    float z = analogRead(sensorPin + 2);
    
    // Store data for processing
    gestureData[0][gestureIndex] = x;
    gestureData[1][gestureIndex] = y;
    gestureData[2][gestureIndex] = z;
    
    gestureIndex = (gestureIndex + 1) % 10;
    
    if (millis() - lastGestureTime > 500) {
        processGestureData();
        lastGestureTime = millis();
    }
    
    return NONE_GESTURE;
}

void GestureControl::handleGesture(GestureType gesture) {
    switch (gesture) {
        case SWIPE_LEFT:
            actuators.setLight(0); // Turn off lights
            break;
            
        case SWIPE_RIGHT:
            actuators.setLight(255); // Turn on lights
            break;
            
        case SWIPE_UP:
            actuators.increaseFanSpeed();
            break;
            
        case SWIPE_DOWN:
            actuators.decreaseFanSpeed();
            break;
            
        case CIRCLE:
            automation.toggleMode("auto");
            break;
            
        case WAVE:
            actuators.toggleDoor();
            break;
            
        case HOLD:
            automation.activateEmergencyMode();
            break;
    }
}

void GestureControl::setGestureSensitivity(float newSensitivity) {
    sensitivity = constrain(newSensitivity, 0.1, 2.0);
}

void GestureControl::calibrateSensor() {
    // Calibration process
    float sumX = 0, sumY = 0, sumZ = 0;
    
    for (int i = 0; i < 100; i++) {
        sumX += analogRead(sensorPin);
        sumY += analogRead(sensorPin + 1);
        sumZ += analogRead(sensorPin + 2);
        delay(10);
    }
    
    // Set baseline values
    float baselineX = sumX / 100;
    float baselineY = sumY / 100;
    float baselineZ = sumZ / 100;
    
    isCalibrated = true;
}

void GestureControl::mapGestureToAction(GestureType gesture, const String& action) {
    if (validateGesture(gesture)) {
        updateGestureMapping(gesture, action);
    }
}

void GestureControl::processGestureData() {
    // Process collected gesture data
    float confidence = calculateGestureConfidence(gestureData[0]);
    
    // Reset data
    gestureIndex = 0;
}

float GestureControl::calculateGestureConfidence(const float* data) {
    // Calculate confidence level of gesture detection
    return 0.95; // Simplified
}

void GestureControl::updateGestureMapping(GestureType gesture, const String& action) {
    // Update gesture-to-action mapping
}

bool GestureControl::validateGesture(GestureType gesture) {
    return gesture != NONE_GESTURE;
}
asa