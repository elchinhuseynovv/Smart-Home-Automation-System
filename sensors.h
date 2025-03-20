#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include <DHT.h>
#include <Wire.h>
#include <BH1750.h>
#include <Adafruit_BMP280.h>
#include <MQ135.h>

class Sensors {
public:
    Sensors(uint8_t dhtPin, uint8_t pirPin, uint8_t ldrPin);
    void begin();
    
    // Environmental readings
    float getTemperature();
    float getHumidity();
    float getPressure();
    float getAltitude();
    
    // Motion and light
    bool getMotion();
    int getLightLevel();
    float getPreciseLightLevel();
    
    // Weather and air quality
    bool isRaining();
    float getAirQuality();
    float getCO2Level();
    float getGasLevel();
    
    // Advanced readings with averaging
    float getAverageTemperature(int samples = 5);
    float getAverageHumidity(int samples = 5);
    float getAveragePressure(int samples = 5);
    float getAverageAirQuality(int samples = 5);
    
    // Trend analysis
    float getTemperatureTrend();
    float getHumidityTrend();
    float getPressureTrend();
    
    // Calibration
    void calibrateAirSensor();
    void calibratePressureSensor();
    
private:
    DHT dht;
    uint8_t pirPin;
    uint8_t ldrPin;
    uint8_t rainPin;
    BH1750 lightMeter;
    Adafruit_BMP280 bmp;
    MQ135 airSensor;
    
    unsigned long lastMotionTime;
    bool motionState;
    
    // History tracking for trends
    static const int HISTORY_SIZE = 12;
    float tempHistory[HISTORY_SIZE];
    float humidityHistory[HISTORY_SIZE];
    float pressureHistory[HISTORY_SIZE];
    int historyIndex;
    
    // Sensor calibration values
    float airQualityBaseline;
    float pressureBaseline;
    float seaLevelPressure;
    
    // Helper methods
    float calculateAverage(float readings[], int count);
    float calculateTrend(float history[], int count);
    void updateHistory(float value, float history[]);
};

#endif