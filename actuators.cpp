#include "actuators.h"

Actuators::Actuators(uint8_t ledPin, uint8_t fanPin, uint8_t buzzerPin, uint8_t servoPin, uint8_t windowServoPin)
    : ledPin(ledPin), fanPin(fanPin), buzzerPin(buzzerPin),
      currentLightLevel(0), currentFanSpeed(OFF), currentDoorState(LOCKED),
      currentWindowOpening(0), systemActive(true), autoFanMode(false),
      fanTempThreshold(25.0), lastDoorOperation(0), autoCloseTime(0) {
    doorServo.attach(servoPin);
    windowServo.attach(windowServoPin);
}

void Actuators::begin() {
    pinMode(ledPin, OUTPUT);
    pinMode(fanPin, OUTPUT);
    pinMode(buzzerPin, OUTPUT);
    setDoorState(LOCKED);
    setWindowOpening(0);
}

void Actuators::setLight(int brightness) {
    brightness = constrain(brightness, 0, 255);
    analogWrite(ledPin, brightness);
    currentLightLevel = brightness;
}

void Actuators::fadeLight(int targetBrightness, int duration) {
    targetBrightness = constrain(targetBrightness, 0, 255);
    int steps = abs(targetBrightness - currentLightLevel);
    int delayTime = duration / steps;
    
    if (targetBrightness > currentLightLevel) {
        for (int i = currentLightLevel; i <= targetBrightness; i++) {
            analogWrite(ledPin, i);
            delay(delayTime);
        }
    } else {
        for (int i = currentLightLevel; i >= targetBrightness; i--) {
            analogWrite(ledPin, i);
            delay(delayTime);
        }
    }
    currentLightLevel = targetBrightness;
}

void Actuators::pulseLight(int duration) {
    fadeLight(255, duration/2);
    fadeLight(0, duration/2);
}

void Actuators::setFan(FanSpeed speed) {
    if (!systemActive) return;
    analogWrite(fanPin, speed);
    currentFanSpeed = speed;
}

void Actuators::setFanAutoMode(bool enabled, float tempThreshold) {
    autoFanMode = enabled;
    fanTempThreshold = tempThreshold;
}

void Actuators::triggerBuzzer(unsigned long duration) {
    if (!systemActive) return;
    digitalWrite(buzzerPin, HIGH);
    delay(duration);
    digitalWrite(buzzerPin, LOW);
}

void Actuators::playMelody(const int* notes, const int* durations, int count) {
    if (!systemActive) return;
    for (int i = 0; i < count; i++) {
        tone(buzzerPin, notes[i], durations[i]);
        delay(durations[i] * 1.1); // 10% pause between notes
    }
    noTone(buzzerPin);
}

void Actuators::stopBuzzer() {
    noTone(buzzerPin);
    digitalWrite(buzzerPin, LOW);
}

void Actuators::setDoorState(DoorState state) {
    if (!systemActive) return;
    
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

DoorState Actuators::getDoorState() {
    return currentDoorState;
}

void Actuators::autoCloseDoor(unsigned long delay) {
    if (currentDoorState != LOCKED) {
        autoCloseTime = millis() + delay;
    }
}

void Actuators::setWindowOpening(int percentage) {
    if (!systemActive) return;
    percentage = constrain(percentage, 0, 100);
    int angle = map(percentage, 0, 100, 0, 180);
    windowServo.write(angle);
    currentWindowOpening = percentage;
}

int Actuators::getWindowOpening() {
    return currentWindowOpening;
}

void Actuators::emergencyShutdown() {
    systemActive = false;
    setLight(0);
    setFan(OFF);
    stopBuzzer();
    setDoorState(LOCKED);
    setWindowOpening(0);
}

void Actuators::restoreSystem() {
    systemActive = true;
}

bool Actuators::isSystemActive() {
    return systemActive;
}