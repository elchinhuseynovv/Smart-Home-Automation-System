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
    HOLD,
    DOUBLE_TAP,
    PINCH,
    SPREAD,
    ROTATE_CW,
    ROTATE_CCW,
    ZIGZAG
};

struct GestureConfig {
    float sensitivity;
    int holdDuration;
    int doubleTapInterval;
    float motionThreshold;
    bool enabled;
};

class GestureControl {
public:
    GestureControl(uint8_t sensorPin);
    void begin();
    
    // Enhanced gesture detection
    void updateGestures();
    GestureType detectGesture();
    void handleGesture(GestureType gesture);
    
    // Advanced configuration
    void setGestureSensitivity(float sensitivity);
    void calibrateSensor();
    void mapGestureToAction(GestureType gesture, const String& action);
    void setGestureConfig(const GestureConfig& config);
    
    // New gesture features
    void enableGestureType(GestureType type, bool enabled);
    void setGestureTimeout(unsigned long timeout);
    void setGestureSequence(GestureType sequence[], int length);
    bool recognizeSequence();
    
    // Gesture learning
    void startGestureLearning(const String& gestureName);
    void stopGestureLearning();
    void saveLearnedGesture();
    
    // Gesture analytics
    float getGestureAccuracy();
    void getGestureStats(int& recognized, int& failed);
    String getLastGestureName();
    
