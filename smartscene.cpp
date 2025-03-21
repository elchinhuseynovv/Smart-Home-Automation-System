#include "smart_scenes.h"

SmartScenes::SmartScenes()
    : sceneCount(0), maxScenes(10), transitionDuration(5) {
    scenes = new Scene[maxScenes];
}

void SmartScenes::begin() {
    loadScenes();
}

void SmartScenes::createScene(const String& name, const Scene& settings) {
    if (sceneCount >= maxScenes) return;
    
    if (validateScene(settings)) {
        scenes[sceneCount] = settings;
        scenes[sceneCount].name = name;
        sceneCount++;
        saveScenes();
    }
}

void SmartScenes::activateScene(const String& name) {
    Scene* currentScene = nullptr;
    Scene* targetScene = nullptr;
    
    // Find current and target scenes
    for (int i = 0; i < sceneCount; i++) {
        if (scenes[i].name == name) {
            targetScene = &scenes[i];
            break;
        }
    }
    
    if (targetScene) {
        // Perform smooth transition
        smoothTransition(*currentScene, *targetScene);
        
        // Apply final settings
        actuators.setTemperature(targetScene->temperature);
        actuators.setLight(targetScene->lightLevel);
        actuators.setFan(static_cast<FanSpeed>(targetScene->fanSpeed));
        actuators.setWindowOpening(targetScene->windowsOpen ? 100 : 0);
        actuators.setLightMode(targetScene->lightMode);
    }
}

void SmartScenes::smoothTransition(const Scene& from, const Scene& to) {
    const int steps = 50;
    const int delayTime = (transitionDuration * 1000) / steps;
    
    for (int i = 0; i <= steps; i++) {
        float progress = static_cast<float>(i) / steps;
        interpolateSettings(from, to, progress);
        delay(delayTime);
    }
}

void SmartScenes::interpolateSettings(const Scene& from, const Scene& to, float progress) {
    // Interpolate temperature
    float temp = from.temperature + (to.temperature - from.temperature) * progress;
    actuators.setTemperature(temp);
    
    // Interpolate light level
    int light = from.lightLevel + (to.lightLevel - from.lightLevel) * progress;
    actuators.setLight(light);
    
    // Interpolate fan speed
    int fan = from.fanSpeed + (to.fanSpeed - from.fanSpeed) * progress;
    actuators.setFan(static_cast<FanSpeed>(fan));
}

void SmartScenes::scheduleScene(const String& name, int hour, int minute) {
    // Add scene to scheduler
    automation.addScheduledTask(name, hour, minute, [this, name]() {
        this->activateScene(name);
    });
}

float SmartScenes::getSceneEfficiency(const String& name) {
    float efficiency = 0.0;
    
    for (int i = 0; i < sceneCount; i++) {
        if (scenes[i].name == name) {
            // Calculate energy efficiency
            float energyUse = scenes[i].lightLevel / 255.0;
            energyUse += scenes[i].fanSpeed / 3.0;
            energyUse += abs(scenes[i].temperature - 22) / 10.0;
            
            efficiency = 100 - (energyUse * 33.33);
            break;
        }
    }
    
    return efficiency;
}

void SmartScenes::optimizeScene(const String& name) {
    for (int i = 0; i < sceneCount; i++) {
        if (scenes[i].name == name) {
            Scene& scene = scenes[i];
            
            // Optimize temperature based on time of day
            int hour = automation.getCurrentHour();
            if (hour >= 22 || hour < 6) {
                scene.temperature = 20.0; // Night temperature
            } else {
                scene.temperature = 23.0; // Day temperature
            }
            
            // Optimize light level based on natural light
            float naturalLight = sensors.getPreciseLightLevel();
            scene.lightLevel = map(naturalLight, 0, 1000, 255, 50);
            
            // Optimize fan speed based on temperature
            float currentTemp = sensors.getTemperature();
            if (currentTemp > scene.temperature + 2) {
                scene.fanSpeed = HIGH;
            } else if (currentTemp > scene.temperature) {
                scene.fanSpeed = MEDIUM;
            } else {
                scene.fanSpeed = LOW;
            }
            
            saveScenes();
            break;
        }
    }
}

bool SmartScenes::validateScene(const Scene& scene) {
    return scene.temperature >= 16 && scene.temperature <= 30 &&
           scene.lightLevel >= 0 && scene.lightLevel <= 255 &&
           scene.fanSpeed >= OFF && scene.fanSpeed <= HIGH;
}

void SmartScenes::saveScenes() {
    // Save scenes to EEPROM or storage
}

void SmartScenes::loadScenes() {
    // Load scenes from EEPROM or storage
}
as
00a