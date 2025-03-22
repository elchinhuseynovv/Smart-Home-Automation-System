#ifndef AUTOMATION_H
#define AUTOMATION_H

#include <Arduino.h>
#include <vector>
#include "sensors.h"
#include "actuators.h"

enum CommandType {
    NONE,
    SET_MODE,
    SET_THRESHOLD,
    CONTROL_DEVICE,
    UPDATE_SCHEDULE,
    SCENE_CONTROL,
    AUTOMATION_RULE
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
    float peakUsage;
    float offPeakUsage;
    float renewableUsage;
};

struct SecurityEvent {
    unsigned long timestamp;
    bool motion;
    float lightLevel;
    String location;
    int severity;
};

struct ComfortFactors {
    float temperature;
    float humidity;
    float airQuality;
    float light;
    float noise;
    float pressure;
};

struct AutomationRule {
    String condition;
    String action;
    bool enabled;
    unsigned long lastTriggered;
    int priority;
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
    
