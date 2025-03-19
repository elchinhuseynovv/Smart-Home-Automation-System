#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include <DHT.h>
#include <Wire.h>
#include <BH1750.h>

class Sensors {
public:
    Sensors(uint8_t dhtPin, uint8_t pirPin, uint8_t ldrPin);
    void begin();
    float getTemperature();
    float getHumidity();
    bool getMotion();
    int getLightLevel();
    float getPreciseLightLevel(); // Precise lux measurement
    bool isRaining();
    float getAirQuality();
    
    // Averaging methods for more stable readings
    float getAverageTemperature(int samples = 5);
    float getAverageHumidity(int samples = 5);
    
private:
    DHT dht;
    uint8_t pirPin;
    uint8_t ldrPin;
    uint8_t rainPin;
    BH1750 lightMeter;
    unsigned long lastMotionTime;
    bool motionState;
    
    // Helper methods
    float calculateAverage(float readings[], int count);
};

#endif