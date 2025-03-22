#ifndef ACTUATORS_H
#define ACTUATORS_H

#include <Arduino.h>
#include <Servo.h>
#include <FastLED.h>

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

enum LightMode {
    NORMAL,
    AMBIENT,
    NIGHT,
    PARTY,
    ALERT
};

class Actuators {
public:
    Actuators(uint8_t ledPin, uint8_t fanPin, uint8_t buzzerPin, uint8_t servoPin, uint8_t windowServoPin);
    void begin();
    
    // Enhanced lighting control
    void setLight(int brightness);
    void fadeLight(int targetBrightness, int duration);
    void pulseLight(int duration);
    void setLightMode(LightMode mode);
    void setAmbientColor(uint8_t r, uint8_t g, uint8_t b);
    void startLightShow(int duration);
    
    // Advanced fan control
    void setFan(FanSpeed speed);
    void setFanAutoMode(bool enabled, float tempThreshold);
    void setFanSchedule(int startHour, int endHour, FanSpeed speed);
    void updateFanControl(float temperature, float humidity);
    
    // Extended buzzer functionality
    void triggerBuzzer(unsigned long duration);
    void playMelody(const int* notes, const int* durations, int count);
    void stopBuzzer();
    void setAlarm(int hour, int minute, const int* melody, const int* durations, int count);
    
    // Smart door control
    void setDoorState(DoorState state);
    DoorState getDoorState();
    void autoCloseDoor(unsigned long delay);
    void setDoorSchedule(int openHour, int closeHour);
    
    // Window control
    void setWindowOpening(int percentage);
    int getWindowOpening();
    void setWindowSchedule(int openHour, int closeHour);
    void updateWindowControl(float temperature, bool isRaining);
    
    // System control
    void emergencyShutdown();
    void restoreSystem();
    bool isSystemActive();
    void setNightMode(bool enabled);
    void setVacationMode(bool enabled);
    
private:
    uint8_t ledPin;
    uint8_t fanPin;
    uint8_t buzzerPin;
    Servo doorServo;
    Servo windowServo;
    CRGB* leds;
    
    // System states
    int currentLightLevel;
    FanSpeed currentFanSpeed;
    DoorState currentDoorState;
    int currentWindowOpening;
    bool systemActive;
    bool autoFanMode;
    bool nightMode;
    bool vacationMode;
    
    // Control parameters
    float fanTempThreshold;
    unsigned long lastDoorOperation;
    unsigned long autoCloseTime;
    
    // Scheduling
    struct Schedule {
        int startHour;
        int endHour;
        bool enabled;
        union {
            FanSpeed fanSpeed;
            int windowOpening;
            DoorState doorState;
        };
    };
    
    Schedule fanSchedule;
    Schedule windowSchedule;
    Schedule doorSchedule;
    
    // Light show parameters
    struct LightShow {
        bool active;
        unsigned long startTime;
        unsigned long duration;
        uint8_t currentPattern;
    } lightShow;
    
    // Helper methods
    void updateLightShow();
    void handleSchedules();
    void checkAlarms();
};

#endif