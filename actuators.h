#ifndef ACTUATORS_H
#define ACTUATORS_H

#include <Arduino.h>
#include <Servo.h>

class Actuators {
public:
    Actuators(uint8_t ledPin, uint8_t fanPin, uint8_t buzzerPin, uint8_t servoPin);
    void begin();
    void setLight(int brightness);
    void setFan(bool state);
    void triggerBuzzer(unsigned long duration);
    void unlockDoor();
    void lockDoor();
    
private:
    uint8_t ledPin;
    uint8_t fanPin;
    uint8_t buzzerPin;
    Servo doorServo;
};