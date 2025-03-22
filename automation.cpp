#include "automation.h"

Automation::Automation()
    : nightMode(false), vacationMode(false), partyMode(false), ecoMode(false),
      tempThreshold(25.0), humidityThreshold(60.0), lightThreshold(300),
      moistureThreshold(40.0), targetTemperature(23.0), targetHumidity(50.0),
      comfortIndex(0.0), baselineConsumption(1000.0), rainExpected(false),
      forecastTemperature(0.0), learningEnabled(true), adaptiveMode(true),
      lastOptimization(0), optimizationInterval(3600000) { // 1 hour
    
    // Initialize energy stats
    energyStats = {0.0, 0.0, 0.0, 0.0, 0.0};
    
    // Initialize learning parameters
    for (int i = 0; i < 24; i++) {
        temperaturePreferences[i] = 23.0;
        lightingPreferences[i] = 200;
        activityPatterns[i] = 0;
    }
}

void Automation::begin() {
    calculateEnergySavings();
    loadUserPreferences();
    initializeML();
}

void Automation::handleClimateControl(const SensorData& data, const WeatherData& forecast) {
    // Predictive climate control with machine learning
    float predictedTemp = mlModel.predictTemperature(data, forecast);
    float optimalTemp = calculateOptimalTemperature(data.temperature, forecast);
    
    // Advanced thermal management
    if (abs(data.temperature - optimalTemp) > 0.5) {
        if (data.temperature < optimalTemp) {
            if (forecast.temperature > data.temperature + 2) {
                // Natural heating possible
                actuators.setWindowOpening(calculateOptimalOpening(data, forecast));
                actuators.setFan(LOW);
            } else {
                activateHeating(optimalTemp - data.temperature);
            }
        } else {
            if (forecast.temperature < data.temperature - 2) {
                // Natural cooling possible
                actuators.setWindowOpening(calculateOptimalOpening(data, forecast));
                actuators.setFan(MEDIUM);
            } else {
                activateCooling(data.temperature - optimalTemp);
            }
        }
    }
    
    // Smart ventilation with CO2 monitoring
    if (data.co2Level > 1000) {
        actuators.activateVentilation();
        if (!data.isRaining && data.temperature > 15) {
            actuators.setWindowOpening(50);
        }
    }
    
    // Humidity balance with dew point calculation
    float dewPoint = calculateDewPoint(data.temperature, data.humidity);
    if (dewPoint > data.temperature - 2) {
        actuators.activateDehumidifier();
    } else if (data.humidity < 30) {
        actuators.activateHumidifier();
    }
}

void Automation::handleGardenCare(const SensorData& data, const WeatherData& forecast) {
    // Smart irrigation with soil analysis
    if (data.soilMoisture < moistureThreshold) {
        if (!rainExpected && data.temperature < 30) {
            float waterAmount = calculateOptimalWatering(data, forecast);
            actuators.startIrrigation(waterAmount);
        }
    }
    
    // Plant protection system
    if (data.uvIndex > 8.0) {
        actuators.deployShade();
        if (data.soilMoisture < moistureThreshold + 10) {
            actuators.startMisting(); // Prevent leaf burn
        }
    }
    
    // Frost protection with temperature gradient monitoring
    if (forecast.temperature < 2.0) {
        actuators.activateFrostProtection();
        actuators.startSoilHeating();
    }
    
    // Disease prevention
    if (data.humidity > 85 && data.temperature > 20) {
        actuators.activateAirCirculation();
        actuators.adjustGreenhouseVents(30);
    }
}

void Automation::handleEnergyManagement(const SensorData& data) {
    static unsigned long lastPeakCheck = 0;
    float currentConsumption = 0.0;
    
    // Real-time load balancing
    if (millis() - lastPeakCheck > 300000) { // 5-minute intervals
        if (isPeakHour()) {
            shiftLoads();
        }
        lastPeakCheck = millis();
    }
    
    // Smart appliance coordination
    if (currentConsumption > baselineConsumption * 1.2) {
        prioritizeLoads();
    }
    
    // Renewable energy optimization
    if (getSolarProduction() > currentConsumption) {
        storeExcessEnergy();
    }
    
    // Adaptive baseline calculation
    updateBaselineConsumption();
    
    // Update energy statistics with machine learning predictions
    updateEnergyStats(currentConsumption);
    predictFutureConsumption();
}

void Automation::handleSecurity(const SensorData& data) {
    static unsigned long lastMotion = 0;
    static int motionCount = 0;
    static std::vector<SecurityEvent> securityLog;
    
    // Advanced motion analysis
    if (data.motion) {
        SecurityEvent event = {millis(), data.motion, data.lightLevel};
        securityLog.push_back(event);
        
        if (analyzeMotionPattern(securityLog)) {
            handleSuspiciousActivity();
        }
        
        if (vacationMode) {
            handleEmergency("SECURITY");
            return;
        }
    }
    
    // Perimeter monitoring
    if (checkPerimeterBreach(data)) {
        activateSecurityResponse();
    }
    
    // Environmental hazard detection
    if (data.airQuality < 30 || data.gasLevel > 100) {
        handleEmergency("ENVIRONMENTAL");
    }
    
    // Smart camera control
    updateCameraCoverage(data.motion);
    
    // Regular security checks
    if (millis() - lastSecurityCheck > 3600000) { // Hourly
        performSecurityAudit();
    }
}

void Automation::optimizeComfort(const SensorData& data) {
    // Multi-factor comfort analysis
    ComfortFactors factors;
    factors.temperature = 1.0 - abs(data.temperature - targetTemperature) / 10.0;
    factors.humidity = 1.0 - abs(data.humidity - targetHumidity) / 30.0;
    factors.airQuality = data.airQuality / 100.0;
    factors.light = calculateLightComfort(data.lightLevel);
    factors.noise = calculateNoiseComfort(data.noiseLevel);
    
    // Calculate weighted comfort index
    comfortIndex = calculateWeightedComfort(factors);
    
    // Adaptive comfort optimization
    if (learningEnabled) {
        updateComfortPreferences(data, comfortIndex);
    }
    
    // Implement comfort improvements
    if (comfortIndex < 70) {
        prioritizeComfortImprovements(factors);
    }
    
    // Circadian rhythm optimization
    adjustLightingForTimeOfDay();
    
    // Air quality management
    if (data.airQuality < 80) {
        improveAirQuality(data);
    }
}

void Automation::setMode(const String& mode, bool enabled) {
    if (mode == "night") {
        nightMode = enabled;
        if (enabled) {
            targetTemperature = 20.0;
            actuators.setLightMode(NIGHT);
        }
    } else if (mode == "vacation") {
        vacationMode = enabled;
        if (enabled) {
            actuators.setSecurityMode(HIGH);
            actuators.setDoorState(LOCKED);
        }
    } else if (mode == "party") {
        partyMode = enabled;
        if (enabled) {
            actuators.setLightMode(PARTY);
            targetTemperature = 22.0;
        }
    } else if (mode == "eco") {
        ecoMode = enabled;
        if (enabled) {
            targetTemperature = 24.0; // Higher in summer
            actuators.setEnergyMode(SAVING);
        }
    }
}

void Automation::updateWeatherStrategy(const WeatherData& forecast) {
    rainExpected = forecast.rainProbability > 70;
    forecastTemperature = forecast.temperature;
    
    // Adjust system behavior based on forecast
    if (rainExpected) {
        actuators.prepareForRain();
    }
    
    if (forecast.temperature > 30) {
        actuators.prepareForHeat();
    } else if (forecast.temperature < 5) {
        actuators.prepareForCold();
    }
}

void Automation::handleCommand(const Command& cmd) {
    switch (cmd.type) {
        case SET_MODE:
            setMode(cmd.target, cmd.value > 0);
            break;
            
        case SET_THRESHOLD:
            if (cmd.target == "temperature") tempThreshold = cmd.value;
            else if (cmd.target == "humidity") humidityThreshold = cmd.value;
            else if (cmd.target == "light") lightThreshold = cmd.value;
            else if (cmd.target == "moisture") moistureThreshold = cmd.value;
            break;
            
        case CONTROL_DEVICE:
            // Handle direct device control
            break;
            
        case UPDATE_SCHEDULE:
            // Handle schedule updates
            break;
    }
}

EnergyStats Automation::getEnergyStats() const {
    return energyStats;
}

float Automation::getComfortIndex() const {
    return comfortIndex;
}

void Automation::adjustClimateControl(float temperature, float humidity) {
    float tempDiff = temperature - targetTemperature;
    
    if (abs(tempDiff) > 3.0) {
        actuators.setFan(HIGH);
    } else if (abs(tempDiff) > 1.0) {
        actuators.setFan(MEDIUM);
    } else if (abs(tempDiff) > 0.5) {
        actuators.setFan(LOW);
    } else {
        actuators.setFan(OFF);
    }
    
    if (humidity > humidityThreshold + 10) {
        actuators.activateDehumidifier();
    } else if (humidity < humidityThreshold - 10) {
        actuators.activateHumidifier();
    }
}

void Automation::calculateEnergySavings() {
    float currentUsage = energyStats.currentConsumption;
    float savings = ((baselineConsumption - currentUsage) / baselineConsumption) * 100;
    energyStats.savingsPercentage = savings > 0 ? savings : 0;
}

void Automation::handleEmergency(const String& type) {
    if (type == "SECURITY") {
        actuators.setLightMode(ALERT);
        actuators.triggerAlarm();
        actuators.lockDownPerimeter();
        notifyAuthorities();
        display.showAlert("Security Alert!");
        logSecurityEvent();
    } else if (type == "ENVIRONMENTAL") {
        actuators.activateEmergencyVentilation();
        actuators.shutOffGasSupply();
        actuators.setWindowOpening(100);
        display.showAlert("Environmental Hazard!");
        evacuationProtocol();
    }
}

void Automation::addAutomationRule(const AutomationRule& rule) {
    rules.push_back(rule);
    std::sort(rules.begin(), rules.end(), 
              [](const AutomationRule& a, const AutomationRule& b) {
                  return a.priority > b.priority;
              });
}

void Automation::removeAutomationRule(const String& condition) {
    rules.erase(
        std::remove_if(rules.begin(), rules.end(),
                      [&](const AutomationRule& rule) {
                          return rule.condition == condition;
                      }),
        rules.end());
}

void Automation::processAutomationRules() {
    unsigned long currentTime = millis();
    for (auto& rule : rules) {
        if (!rule.enabled) continue;
        
        if (evaluateCondition(rule.condition)) {
            if (currentTime - rule.lastTriggered > 60000) { // 1-minute minimum interval
                executeAction(rule.action);
                rule.lastTriggered = currentTime;
            }
        }
    }
}

void Automation::optimizeEnergyUsage(bool enableML) {
    if (enableML) {
        // Use machine learning to predict optimal settings
        float predictedUsage = mlModel.predictEnergyUsage();
        float optimalTemp = mlModel.getOptimalTemperature();
        
        targetTemperature = optimalTemp;
        adjustHVACSchedule(predictedUsage);
    }
    
    // Implement peak load shifting
    if (isPeakHour()) {
        deferNonEssentialLoads();
        activateStoredEnergy();
    }
    
    // Optimize renewable energy usage
    if (getSolarProduction() > baselineConsumption * 0.8) {
        scheduleHighConsumptionTasks();
    }
}

void Automation::analyzeBehaviorPatterns() {
    static std::vector<ActivityData> activityLog;
    
    // Update activity patterns
    int currentHour = (millis() / 3600000) % 24;
    activityPatterns[currentHour]++;
    
    // Analyze patterns
    if (activityLog.size() >= 168) { // One week of hourly data
        updateBehaviorModel(activityLog);
        activityLog.clear();
    }
}

void Automation::predictMaintenanceNeeds() {
    // Analyze system performance metrics
    SystemMetrics metrics = collectSystemMetrics();
    
    // Predict maintenance needs using ML model
    MaintenancePrediction prediction = mlModel.predictMaintenance(metrics);
    
    if (prediction.requiresMaintenance) {
        scheduleMaintenance(prediction);
        notifyMaintenanceNeeded(prediction.components);
    }
}

void Automation::generateEfficiencyReport() {
    EfficiencyReport report;
    
    // Calculate energy efficiency
    report.energyEfficiency = calculateEnergyEfficiency();
    report.waterEfficiency = calculateWaterEfficiency();
    report.hvacEfficiency = calculateHVACEfficiency();
    
    // Generate recommendations
    report.recommendations = generateEfficiencyRecommendations();
    
    // Store report
    storeEfficiencyReport(report);
}

void Automation::handleEmergencyScenario(const String& scenario) {
    EmergencyProtocol protocol = determineEmergencyProtocol(scenario);
    
    switch (protocol.type) {
        case FIRE:
            activateFireProtocol();
            break;
        case FLOOD:
            activateFloodProtocol();
            break;
        case SECURITY_BREACH:
            activateSecurityProtocol();
            break;
        case POWER_FAILURE:
            activateBackupPower();
            break;
    }
    
    notifyEmergencyContacts(protocol);
}

void Automation::updateAIModel(const SensorData& data) {
    // Update training data
    mlModel.addTrainingData(data);
    
    // Periodically retrain model
    if (millis() - lastModelUpdate > modelUpdateInterval) {
        mlModel.retrain();
        lastModelUpdate = millis();
    }
    
    // Update prediction accuracy metrics
    updateModelAccuracy();
}

void Automation::adjustForSeasonalChanges() {
    Season currentSeason = determineCurrentSeason();
    
    // Adjust comfort parameters
    adjustComfortParameters(currentSeason);
    
    // Update energy baselines
    updateSeasonalBaselines(currentSeason);
    
    // Modify automation rules
    updateSeasonalRules(currentSeason);
}

