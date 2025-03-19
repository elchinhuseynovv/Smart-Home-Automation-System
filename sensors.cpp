#include "sensors.h"

Sensors::Sensors(uint8_t dhtPin, uint8_t pirPin, uint8_t ldrPin) 
    : dht(dhtPin, DHT11), pirPin(pirPin), ldrPin(ldrPin) {
}

void Sensors::begin() {
    dht.begin();
    pinMode(pirPin, INPUT);
    pinMode(ldrPin, INPUT);
}

float Sensors::getTemperature() {
    return dht.readTemperature();
}

float Sensors::getHumidity() {
    return dht.readHumidity();
}

bool Sensors::getMotion() {
    return digitalRead(pirPin) == HIGH;
}

int Sensors::getLightLevel() {
    return analogRead(ldrPin);
}