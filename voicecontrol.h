#ifndef VOICE_CONTROL_H
#define VOICE_CONTROL_H

#include <Arduino.h>
#include "automation.h"

enum VoiceCommand {
    NONE_CMD,
    LIGHTS_ON,
    LIGHTS_OFF,
    SET_TEMPERATURE,
    OPEN_WINDOWS,
    CLOSE_WINDOWS,
    SECURITY_STATUS,
    ENERGY_REPORT,
    WEATHER_REPORT
};

class VoiceControl {
public:
    VoiceControl();
    void begin();