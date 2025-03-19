#ifndef ACTUATORS_H
#define ACTUATORS_H

#include <Arduino.h>
#include <Servo.h>

enum FanSpeed {
    OFF = 0,
    LOW = 85,
    MEDIUM = 170,
    HIGH = 255
};

enum DoorState {
    LOCKED,
    UNLOCKED,
    PARTIALLY_OPEN
};

class Actuators {
public:
    Actuators(uint8_t ledPin, uint8_t fanPin, uint8_t buzzerPin, uint8_t servoPin, uint8_t windowServoPin);
    void begin();
    
    // Enhanced lighting control
    void setLight(int brightness);
    void fadeLight(int targetBrightness, int duration);
    void pulseLight(int duration);
    
    // Advanced fan control
    void setFan(FanSpeed speed);
    void setFanAutoMode(bool enabled, float tempThreshold);
    
    // Extended buzzer functionality
    void triggerBuzzer(unsigned long duration);
    void playMelody(const int* notes, const int* durations, int count);
    void stopBuzzer();
    
    // Smart door control
    void setDoorState(DoorState state);
    DoorState getDoorState();
    void autoCloseDoor(unsigned long delay);
    
    // Window control
    void setWindowOpening(int percentage);
    int getWindowOpening();
    
    // System control
    void emergencyShutdown();
    void restoreSystem();
    bool isSystemActive();
    
private:
    uint8_t ledPin;
    uint8_t fanPin;
    uint8_t buzzerPin;
    Servo doorServo;
    Servo windowServo;
    
    int currentLightLevel;
    FanSpeed currentFanSpeed;
    DoorState currentDoorState;
    int currentWindowOpening;
    bool systemActive;
    bool autoFanMode;
    float fanTempThreshold;
    
    unsigned long lastDoorOperation;
    unsigned long autoCloseTime;
    
    void updateFan();
};