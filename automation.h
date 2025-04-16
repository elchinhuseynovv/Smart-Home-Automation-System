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

// New structures for enhanced functionality
struct MaintenanceSchedule {
    String component;
    unsigned long lastMaintenance;
    unsigned long nextMaintenance;
    int priority;
    bool completed;
};

struct PowerUsageProfile {
    float baselineUsage;
    float peakHourUsage;
    float offPeakUsage;
    float renewableContribution;
    float batteryCapacity;
};

struct WeatherAdaptation {
    float tempAdjustment;
    float humidityAdjustment;
    bool windowStrategy;
    bool shadeControl;
    float energyImpact;
};

struct OccupancyPattern {
    int hourlyPresence[24];
    float activityLevels[24];
    bool weekendPattern;
    unsigned long lastUpdate;
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
    
    // Advanced Features
    void addAutomationRule(const AutomationRule& rule);
    void removeAutomationRule(const String& condition);
    void processAutomationRules();
    void optimizeEnergyUsage(bool enableML = true);
    void analyzeBehaviorPatterns();
    void predictMaintenanceNeeds();
    void generateEfficiencyReport();
    void handleEmergencyScenario(const String& scenario);
    void updateAIModel(const SensorData& data);
    void adjustForSeasonalChanges();
    void manageLoadBalancing();
    void optimizeHVACSchedule();
    String getSecurityStatus() const;
    void notifyAuthorities();
    void evacuationProtocol();

    // New Advanced Functions
    void predictOccupancy(const OccupancyPattern& pattern);
    void optimizePowerDistribution(const PowerUsageProfile& profile);
    void schedulePreventiveMaintenance(const MaintenanceSchedule& schedule);
    void adaptToWeatherChanges(const WeatherAdaptation& adaptation);
    void manageSmartAppliances();
    void optimizeWaterUsage();
    void monitorStructuralHealth();
    void controlSmartBlinds(float sunIntensity);
    void manageEVCharging(bool peakHours);
    void optimizeVentilation(float co2Level);
    
    // New Comfort Functions
    void setZoneTemperature(int zone, float temperature);
    void adjustAmbientLighting(float intensity, int colorTemp);
    void manageAirQuality(float pollutionLevel);
    void controlHumidification(float targetHumidity);
    void optimizeAcousticComfort(float noiseLevel);
    
    // New Energy Functions
    void manageSolarProduction(float currentProduction);
    void optimizeBatteryStorage(float chargeLevel);
    void shiftPeakLoads(float currentDemand);
    void monitorApplianceEfficiency();
    void trackEnergyUsagePatterns();
    
    // New Security Functions
    void monitorPerimeterSecurity();
    void detectAnomalies(const SecurityEvent& event);
    void manageCCTVRecording();
    void controlAccessPoints();
    void handleSecurityBreach(const String& location);

private:
    // System states
    bool nightMode;
    bool vacationMode;
    bool partyMode;
    bool ecoMode;
    bool learningEnabled;
    bool adaptiveMode;
    
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
    
    // Learning parameters
    float temperaturePreferences[24];
    int lightingPreferences[24];
    int activityPatterns[24];
    unsigned long lastOptimization;
    unsigned long optimizationInterval;
    unsigned long lastSecurityCheck;
    