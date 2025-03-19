#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include <DHT.h>

class Sensors {
public:
    Sensors(uint8_t dhtPin, uint8_t pirPin, uint8_t ldrPin);
    void begin();
    float getTemperature();
    float getHumidity();
    bool getMotion();
    int getLightLevel();
    
private:
    DHT dht;
    uint8_t pirPin;
    uint8_t ldrPin;
};

#endif