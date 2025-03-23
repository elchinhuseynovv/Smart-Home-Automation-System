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
    
private:
    uint8_t sensorPin;
    float sensitivity;
    bool isCalibrated;
    GestureConfig config;
    
    // Gesture processing
    float gestureData[3][10]; // Store x,y,z coordinates
    int gestureIndex;
    unsigned long lastGestureTime;
    unsigned long gestureTimeout;
    
    // Gesture learning
    bool isLearning;
    String learningGestureName;
    std::vector<float> learningData;
    
    // Gesture statistics
    int recognizedGestures;
    int failedGestures;
    String lastGesture;
    
    // Helper methods
    void processGestureData();
    float calculateGestureConfidence(const float* data);
    void updateGestureMapping(GestureType gesture, const String& action);
    bool validateGesture(GestureType gesture);
    void processSequence(GestureType gesture);
    float compareGesturePatterns(const float* pattern1, const float* pattern2, int length);
    void updateGestureStats(bool recognized);
};

#endif