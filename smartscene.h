#ifndef SMART_SCENES_H
#define SMART_SCENES_H

#include <Arduino.h>
#include "automation.h"

struct Scene {
    String name;
    float temperature;
    int lightLevel;
    int fanSpeed;
    bool windowsOpen;
    LightMode lightMode;
    String description;
};

class SmartScenes {
public:
    SmartScenes();
    void begin();
    
    // Scene management
    void createScene(const String& name, const Scene& settings);
    void activateScene(const String& name);
    void deleteScene(const String& name);
    void modifyScene(const String& name, const Scene& newSettings);
    
    // Scene scheduling
    void scheduleScene(const String& name, int hour, int minute);
    void cancelSchedule(const String& name);
    
    // Scene transitions
    void setTransitionDuration(int seconds);
    void smoothTransition(const Scene& from, const Scene& to);
    
    // Scene analysis
    float getSceneEfficiency(const String& name);
    void optimizeScene(const String& name);
    
private:
    Scene* scenes;
    int sceneCount;
    int maxScenes;
    int transitionDuration;
    
    // Helper methods
    bool validateScene(const Scene& scene);
    void interpolateSettings(const Scene& from, const Scene& to, float progress);
    void saveScenes();
    void loadScenes();
};

#endif

a