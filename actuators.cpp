#include "actuators.h"

Actuators::Actuators(uint8_t ledPin, uint8_t fanPin, uint8_t buzzerPin, uint8_t servoPin)
    : ledPin(ledPin), fanPin(fanPin), buzzerPin(buzzerPin) {
    doorServo.attach(servoPin);
}

void Actuators::begin() {
    pinMode(ledPin, OUTPUT);
    pinMode(fanPin, OUTPUT);
    pinMode(buzzerPin, OUTPUT);
    lockDoor();
}

void Actuators::setLight(int brightness) {
    analogWrite(ledPin, brightness);
}

void Actuators::setFan(bool state) {
    digitalWrite(fanPin, state ? HIGH : LOW);
}

void Actuators::triggerBuzzer(unsigned long duration) {
    digitalWrite(buzzerPin, HIGH);
    delay(duration);
    digitalWrite(buzzerPin, LOW);
}

void Actuators::unlockDoor() {
    doorServo.write(90);
}

void Actuators::lockDoor() {
    doorServo.write(0);
}