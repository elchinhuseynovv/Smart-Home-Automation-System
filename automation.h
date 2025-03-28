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
    
    // New Advanced Features
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
    
    // Automation rules
    std::vector<AutomationRule> rules;
    
    // Helper methods
    void adjustClimateControl(float temperature, float humidity);
    void calculateEnergySavings();
    void handleEmergency(const String& type);
    void loadUserPreferences();
    void initializeML();
    float calculateOptimalTemperature(float currentTemp, const WeatherData& forecast);
    float calculateOptimalOpening(const SensorData& data, const WeatherData& forecast);
    void activateHeating(float difference);
    void activateCooling(float difference);
    float calculateDewPoint(float temperature, float humidity);
    float calculateOptimalWatering(const SensorData& data, const WeatherData& forecast);
    bool isPeakHour() const;
    void shiftLoads();
    void prioritizeLoads();
    float getSolarProduction() const;
    void storeExcessEnergy();
    void updateBaselineConsumption();
    void updateEnergyStats(float consumption);
    void predictFutureConsumption();
    bool analyzeMotionPattern(const std::vector<SecurityEvent>& events);
    void handleSuspiciousActivity();
    bool checkPerimeterBreach(const SensorData& data);
    void activateSecurityResponse();
    void updateCameraCoverage(bool motion);
    void performSecurityAudit();
    float calculateLightComfort(float lightLevel);
    float calculateNoiseComfort(float noiseLevel);
    float calculateWeightedComfort(const ComfortFactors& factors);
    void updateComfortPreferences(const SensorData& data, float comfort);
    void prioritizeComfortImprovements(const ComfortFactors& factors);
    void adjustLightingForTimeOfDay();
    void improveAirQuality(const SensorData& data);
    void logSecurityEvent();
};

#endif