#ifndef GESTURE_CONTROL_H
#define GESTURE_CONTROL_H

#include <Arduino.h>
#include "automation.h"

enum GestureType {
    NONE_GESTURE,
    SWIPE_LEFT,
    SWIPE_RIGHT,
    SWIPE_UP,
    SWIPE_DOWN,
    CIRCLE,
    WAVE,
    HOLD
};

class GestureControl {
public:
    GestureControl(uint8_t sensorPin);
    void begin();
    
    // Gesture detection
    void updateGestures();
    GestureType detectGesture();
    void handleGesture(GestureType gesture);
    
    // Gesture configuration
    void setGestureSensitivity(float sensitivity);
    void calibrateSensor();
    void mapGestureToAction(GestureType gesture, const String& action);
    
private:
    uint8_t sensorPin;
    float sensitivity;
    bool isCalibrated;
    
    // Gesture processing
    float gestureData[3][10]; // Store x,y,z coordinates
    int gestureIndex;
    unsigned long lastGestureTime;
    
    // Helper methods
    void processGestureData();
    float calculateGestureConfidence(const float* data);
    void updateGestureMapping(GestureType gesture, const String& action);
    bool validateGesture(GestureType gesture);
};

#endif