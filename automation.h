#ifndef AUTOMATION_H
#define AUTOMATION_H

#include <Arduino.h>
#include "sensors.h"
#include "actuators.h"

enum CommandType {
    NONE,
    SET_MODE,
    SET_THRESHOLD,
    CONTROL_DEVICE,
    UPDATE_SCHEDULE
};

struct Command {
    CommandType type;
    String target;
    float value;
    String parameters;
};

struct SystemSettings {
    bool autoMode;
    bool energySaveMode;
    bool gardenMode;
    struct {
        float temperature;
        float humidity;
        float light;
        float moisture;
    } thresholds;
};

struct EnergyStats {
    float currentConsumption;
    float dailyConsumption;
    float weeklyConsumption;
    float monthlyConsumption;
    float savingsPercentage;
};

class Automation {
public:
    Automation();
    void begin();
    
    // Main control functions
    void handleClimateControl(const SensorData& data, const WeatherData& forecast);
    void handleGardenCare(const SensorData& data, const WeatherData& forecast);
    void handleEnergyManagement(const SensorData& data);
    void handleSecurity(const SensorData& data);
    void optimizeComfort(const SensorData& data);
    
    // Mode management
    void setMode(const String& mode, bool enabled);
    void setThresholds(const SystemSettings::thresholds& newThresholds);
    
    // Schedule management
    void updateSchedule(const String& device, const Schedule& schedule);
    void checkSchedules();
    
    // Weather adaptation
    void updateWeatherStrategy(const WeatherData& forecast);
    
    // Command handling
    void handleCommand(const Command& cmd);
    
    // Statistics and reporting
    EnergyStats getEnergyStats() const;
    float getComfortIndex() const;
    
private:
    // System states
    bool nightMode;
    bool vacationMode;
    bool partyMode;
    bool ecoMode;
    
    // Thresholds
    float tempThreshold;
    float humidityThreshold;
    float lightThreshold;
    float moistureThreshold;
    
    // Comfort parameters
    float targetTemperature;
    float targetHumidity;
    float comfortIndex;
    
    // Energy management
    EnergyStats energyStats;
    float baselineConsumption;
    
    // Weather adaptation
    bool rainExpected;
    float forecastTemperature;
    
    // Helper methods
    void adjustClimateControl(float temperature, float humidity);
    void optimizeLighting(float lightLevel, bool motion);
    void manageEnergySaving(const SensorData& data);
    void updateComfortIndex(const SensorData& data);
    void calculateEnergySavings();
    void handleEmergency(const String& type);
};

#endif