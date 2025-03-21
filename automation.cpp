#include "automation.h"

Automation::Automation()
    : nightMode(false), vacationMode(false), partyMode(false), ecoMode(false),
      tempThreshold(25.0), humidityThreshold(60.0), lightThreshold(300),
      moistureThreshold(40.0), targetTemperature(23.0), targetHumidity(50.0),
      comfortIndex(0.0), baselineConsumption(1000.0), rainExpected(false),
      forecastTemperature(0.0) {
    
    // Initialize energy stats
    energyStats = {0.0, 0.0, 0.0, 0.0, 0.0};
}

void Automation::begin() {
    // Initialize baseline values and load saved settings
    calculateEnergySavings();
}

void Automation::handleClimateControl(const SensorData& data, const WeatherData& forecast) {
    // Predictive climate control based on weather forecast
    if (forecast.temperature > data.temperature + 2) {
        // Prepare for temperature increase
        if (data.temperature < targetTemperature) {
            actuators.setFan(LOW);
        }
    } else if (forecast.temperature < data.temperature - 2) {
        // Prepare for temperature decrease
        if (data.temperature > targetTemperature) {
            actuators.setFan(MEDIUM);
        }
    }
    
    // Adjust for current conditions
    adjustClimateControl(data.temperature, data.humidity);
    
    // Handle rain prediction
    if (forecast.rainProbability > 70 && !data.isRaining) {
        actuators.setWindowOpening(0);
        display.showAlert("Rain expected - Windows closed");
    }
}

void Automation::handleGardenCare(const SensorData& data, const WeatherData& forecast) {
    // Smart irrigation control
    if (data.soilMoisture < moistureThreshold) {
        if (!rainExpected && data.temperature < 30) {
            // Water the garden if no rain is expected and it's not too hot
            actuators.startIrrigation();
        }
    }
    
    // UV protection
    if (data.uvIndex > 8.0) {
        actuators.deployShade();
        display.showAlert("High UV - Shade deployed");
    }
    
    // Frost protection
    if (forecast.temperature < 2.0) {
        actuators.activateFrostProtection();
    }
}

void Automation::handleEnergyManagement(const SensorData& data) {
    float currentConsumption = 0.0;
    
    // Optimize HVAC usage
    if (abs(data.temperature - targetTemperature) < 1.0) {
        actuators.setFan(OFF);
        currentConsumption += 0;
    } else {
        currentConsumption += actuators.getFanPower();
    }
    
    // Lighting optimization
    if (data.lightLevel > lightThreshold) {
        actuators.setLight(0);
    } else {
        currentConsumption += actuators.getLightPower();
    }
    
    // Update energy statistics
    energyStats.currentConsumption = currentConsumption;
    calculateEnergySavings();
}

void Automation::handleSecurity(const SensorData& data) {
    static unsigned long lastMotion = 0;
    static int motionCount = 0;
    
    // Motion detection logic
    if (data.motion) {
        if (vacationMode) {
            handleEmergency("SECURITY");
            return;
        }
        
        unsigned long currentTime = millis();
        if (currentTime - lastMotion < 60000) { // Within 1 minute
            motionCount++;
            if (motionCount > 5) { // Excessive motion
                actuators.setLightMode(ALERT);
                display.showAlert("Unusual activity detected!");
            }
        } else {
            motionCount = 1;
        }
        lastMotion = currentTime;
    }
    
    // Environmental security
    if (data.airQuality < 30) {
        actuators.activateVentilation();
        display.showAlert("Poor air quality - Ventilating");
    }
}

void Automation::optimizeComfort(const SensorData& data) {
    // Calculate comfort index based on multiple factors
    float tempFactor = 1.0 - abs(data.temperature - targetTemperature) / 10.0;
    float humidityFactor = 1.0 - abs(data.humidity - targetHumidity) / 30.0;
    float airQualityFactor = data.airQuality / 100.0;
    
    comfortIndex = (tempFactor + humidityFactor + airQualityFactor) / 3.0 * 100.0;
    
    // Adjust systems based on comfort index
    if (comfortIndex < 60) {
        // Comfort is low, take corrective action
        if (data.temperature != targetTemperature) {
            adjustClimateControl(data.temperature, data.humidity);
        }
        if (data.airQuality < 50) {
            actuators.activateVentilation();
        }
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
        display.showAlert("Security Alert!");
        // Additional security measures
    }
}