#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include <DHT.h>
#include <Wire.h>
#include <BH1750.h>
#include <Adafruit_BMP280.h>
#include <MQ135.h>
#include <CircularBuffer.h>

// Enhanced sensor calibration structure
struct SensorCalibration {
    float tempOffset;
    float humidityOffset;
    float pressureOffset;
    float lightOffset;
    float airQualityBaseline;
    float uvOffset;
    float soilMoistureOffset;
    float waterLevelOffset;
    unsigned long lastCalibration;
};

// Enhanced sensor thresholds structure
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
    float uvIndexMax;
    float soilMoistureMin;
    float waterLevelMin;
};

// New sensor fusion data structure
struct SensorFusion {
    float temperature;
    float humidity;
    float pressure;
    float confidence;
    unsigned long timestamp;
};

// New maintenance prediction structure
struct MaintenancePrediction {
    bool requiresMaintenance;
    String component;
    float reliability;
    unsigned long predictedTime;
};

class Sensors {
public:
    Sensors(uint8_t dhtPin, uint8_t pirPin, uint8_t ldrPin);
    void begin();
    
    // Enhanced environmental readings
    float getTemperature();
    float getHumidity();
    float getPressure();
    float getAltitude();
    float getDewPoint();
    float getHeatIndex();
    
    // Enhanced motion and light
    bool getMotion();
    int getLightLevel();
    float getPreciseLightLevel();
    float getUVIndex();
    
    // Enhanced weather and air quality
    bool isRaining();
    float getAirQuality();
    float getCO2Level();
    float getGasLevel();
    float getVOCLevel();
    float getOzoneLevel();
    
    // Enhanced readings with averaging
    float getAverageTemperature(int samples = 5);
    float getAverageHumidity(int samples = 5);
    float getAveragePressure(int samples = 5);
    float getAverageAirQuality(int samples = 5);
    
    // Enhanced trend analysis
    float getTemperatureTrend();
    float getHumidityTrend();
    float getPressureTrend();
    float getAirQualityTrend();
    
    // Enhanced environmental metrics
    float getSoilMoisture();
    float getSoilTemperature();
    float getSoilPH();
    float getWaterLevel();
    float getSoundLevel();
    float getRadiationLevel();
    
    // New sensor fusion methods
    SensorFusion getFusedEnvironmentalData();
    float getConfidenceScore();
    void updateSensorFusion();
    
    // New predictive analytics
    float getPredictedTemperature(int hoursAhead);
    float getComfortIndex();
    float getAirQualityIndex();
    bool isPrecipitationLikely();
    MaintenancePrediction getPredictedMaintenance();
    
    // Enhanced calibration and configuration
    void calibrateAllSensors();
    void calibrateAirSensor();
    void calibratePressureSensor();
    void calibrateLightSensor();
    void calibrateUVSensor();
    void setSensorCalibration(const SensorCalibration& calibration);
    void setSensorThresholds(const SensorThresholds& thresholds);
    void setUpdateInterval(unsigned long interval);
    
    // Enhanced diagnostics
    bool performSelfTest();
    float getBatteryLevel();
    bool getSensorStatus(const String& sensorName);
    void getErrorLog(String& log);
    float getSensorReliability(const String& sensorName);
    
    // New data management
    void clearHistory();
    void exportData(String& data);
    bool importData(const String& data);
    
private:
    DHT dht;
    uint8_t pirPin;
    uint8_t ldrPin;
    uint8_t rainPin;
    BH1750 lightMeter;
    Adafruit_BMP280 bmp;
    MQ135 airSensor;
    
    // Enhanced configuration
    SensorCalibration calibration;
    SensorThresholds thresholds;
    unsigned long updateInterval;
    
    // Enhanced state tracking
    unsigned long lastMotionTime;
    bool motionState;
    unsigned long lastUpdate;
    bool sensorError;
    
    // Enhanced history tracking
    static const int HISTORY_SIZE = 24;
    CircularBuffer<float, HISTORY_SIZE> tempHistory;
    CircularBuffer<float, HISTORY_SIZE> humidityHistory;
    CircularBuffer<float, HISTORY_SIZE> pressureHistory;
    CircularBuffer<float, HISTORY_SIZE> airQualityHistory;
    