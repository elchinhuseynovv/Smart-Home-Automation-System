#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include <DHT.h>
#include <Wire.h>
#include <BH1750.h>
#include <Adafruit_BMP280.h>
#include <MQ135.h>

struct SensorCalibration {
    float tempOffset;
    float humidityOffset;
    float pressureOffset;
    float lightOffset;
    float airQualityBaseline;
};

struct SensorThresholds {
    float tempMin;
    float tempMax;
    float humidityMin;
    float humidityMax;
    float pressureMin;
    float pressureMax;
    float lightMin;
    float lightMax;
    float airQualityMin;
};

class Sensors {
public:
    Sensors(uint8_t dhtPin, uint8_t pirPin, uint8_t ldrPin);
    void begin();
    
    // Environmental readings
    float getTemperature();
    float getHumidity();
    float getPressure();
    float getAltitude();
    float getDewPoint();
    float getHeatIndex();
    
    // Motion and light
    bool getMotion();
    int getLightLevel();
    float getPreciseLightLevel();
    float getUVIndex();
    
    // Weather and air quality
    bool isRaining();
    float getAirQuality();
    float getCO2Level();
    float getGasLevel();
    float getVOCLevel();
    float getOzoneLevel();
    
    // Advanced readings with averaging
    float getAverageTemperature(int samples = 5);
    float getAverageHumidity(int samples = 5);
    float getAveragePressure(int samples = 5);
    float getAverageAirQuality(int samples = 5);
    
    // Trend analysis
    float getTemperatureTrend();
    float getHumidityTrend();
    float getPressureTrend();
    float getAirQualityTrend();
    
    // New environmental metrics
    float getSoilMoisture();
    float getSoilTemperature();
    float getSoilPH();
    float getWaterLevel();
    float getSoundLevel();
    float getRadiationLevel();
    
    // Advanced analytics
    float getPredictedTemperature(int hoursAhead);
    float getComfortIndex();
    float getAirQualityIndex();
    bool isPrecipitationLikely();
    
    // Calibration and configuration
    void calibrateAirSensor();
    void calibratePressureSensor();
    void setSensorCalibration(const SensorCalibration& calibration);
    void setSensorThresholds(const SensorThresholds& thresholds);
    void setUpdateInterval(unsigned long interval);
    
    // Sensor diagnostics
    bool performSelfTest();
    float getBatteryLevel();
    bool getSensorStatus(const String& sensorName);
    void getErrorLog(String& log);
    
private:
    DHT dht;
    uint8_t pirPin;
    uint8_t ldrPin;
    uint8_t rainPin;
    BH1750 lightMeter;
    Adafruit_BMP280 bmp;
    MQ135 airSensor;
    
