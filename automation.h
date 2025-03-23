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
    String status;
};

struct EnergyProfile {
    float baselineUsage;
    float peakThreshold;
    float renewableCapacity;
    float storageCapacity;
    float currentEfficiency;
};

struct SecurityZone {
    String name;
    bool armed;
    bool breached;
    std::vector<String> sensors;
    int alertLevel;
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
    void predictiveMaintenanceCheck();
    void scheduleAutomaticMaintenance(const String& component);
    float calculateMaintenanceScore(const String& component);
    void generateMaintenanceReport();
    
    // Enhanced Energy Management
    void optimizePeakLoadDistribution();
    void manageRenewableIntegration();
    float calculateEnergyEfficiency();
    void suggestEnergyOptimizations();
    
    // Advanced Security Features
    void configureSecurityZones(const std::vector<SecurityZone>& zones);
    void handleSecurityBreach(const String& zone);
    void generateSecurityReport();
    bool validateSecurityCredentials(const String& credentials);
    
    // Smart Learning
    void updateLearningModel(const SensorData& data);
    void generateBehaviorInsights();
    float predictUserPreference(const String& parameter);
    void adaptToUserPatterns();
    
    // System Health
    void performSystemDiagnostics();
    void monitorComponentHealth();
    void backupSystemSettings();
    void restoreSystemSettings();
    
private:
    // Existing private members...
    bool nightMode;
    bool vacationMode;
    bool partyMode;
    bool ecoMode;
    bool learningEnabled;
    bool adaptiveMode;
    
    float tempThreshold;
    float humidityThreshold;
    float lightThreshold;
    float moistureThreshold;
    
    float targetTemperature;
    float targetHumidity;
    float comfortIndex;
    
    EnergyStats energyStats;
    float baselineConsumption;
    
    bool rainExpected;
    float forecastTemperature;
    
    float temperaturePreferences[24];
    int lightingPreferences[24];
    int activityPatterns[24];
    unsigned long lastOptimization;
    unsigned long optimizationInterval;
    unsigned long lastSecurityCheck;
    
    std::vector<AutomationRule> rules;
    
    // New private members
    std::vector<MaintenanceSchedule> maintenanceSchedules;
    std::vector<SecurityZone> securityZones;
    EnergyProfile energyProfile;
    
