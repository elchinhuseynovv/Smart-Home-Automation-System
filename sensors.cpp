#include "sensors.h"

Sensors::Sensors(uint8_t dhtPin, uint8_t pirPin, uint8_t ldrPin) 
    : dht(dhtPin, DHT22), // Upgraded to DHT22 for better accuracy
      pirPin(pirPin), 
      ldrPin(ldrPin),
      lastMotionTime(0),
      motionState(false) {
}

void Sensors::begin() {
    dht.begin();
    pinMode(pirPin, INPUT);
    pinMode(ldrPin, INPUT);
    pinMode(rainPin, INPUT);
    Wire.begin();
    lightMeter.begin();
}

float Sensors::getTemperature() {
    float temp = dht.readTemperature();
    return isnan(temp) ? -999 : temp;
}

float Sensors::getHumidity() {
    float humidity = dht.readHumidity();
    return isnan(humidity) ? -999 : humidity;
}

bool Sensors::getMotion() {
    bool currentReading = digitalRead(pirPin) == HIGH;
    unsigned long currentTime = millis();
    
    // Debounce motion detection
    if (currentReading != motionState) {
        if (currentTime - lastMotionTime >= 2000) { // 2-second debounce
            motionState = currentReading;
            lastMotionTime = currentTime;
        }
    }
    
    return motionState;
}

int Sensors::getLightLevel() {
    return analogRead(ldrPin);
}

float Sensors::getPreciseLightLevel() {
    return lightMeter.readLightLevel();
}

bool Sensors::isRaining() {
    return analogRead(rainPin) < 500; // Adjust threshold as needed
}

float Sensors::getAirQuality() {
    // Simulate air quality reading (0-100%, where 100% is best)
    return map(analogRead(A1), 0, 1023, 0, 100);
}

float Sensors::getAverageTemperature(int samples) {
    float readings[samples];
    for (int i = 0; i < samples; i++) {
        readings[i] = getTemperature();
        delay(100);
    }
    return calculateAverage(readings, samples);
}

float Sensors::getAverageHumidity(int samples) {
    float readings[samples];
    for (int i = 0; i < samples; i++) {
        readings[i] = getHumidity();
        delay(100);
    }
    return calculateAverage(readings, samples);
}

float Sensors::calculateAverage(float readings[], int count) {
    float sum = 0;
    int validReadings = 0;
    
    for (int i = 0; i < count; i++) {
        if (readings[i] != -999) {
            sum += readings[i];
            validReadings++;
        }
    }
    
    return validReadings > 0 ? sum / validReadings : -999;
}