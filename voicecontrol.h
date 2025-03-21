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
    
    // Voice recognition
    void processAudioInput();
    VoiceCommand recognizeCommand(const String& audioData);
    void executeCommand(VoiceCommand cmd, const String& parameters);
    
    // Voice synthesis
    void speak(const String& message);
    void playAudioResponse(const String& response);
    
    // Training and calibration
    void trainNewCommand(const String& command, const String& action);
    void calibrateMicrophone();
    
private:
    bool isListening;
    float confidenceThreshold;
    String lastCommand;
    
    // Audio processing
    void filterNoise(float* audioData, int length);
    float calculateConfidence(const String& input, const String& command);
    void updateCommandDatabase(const String& command, const String& pattern);
    
    // Helper methods
    String extractParameters(const String& audioInput);
    void logVoiceActivity(const String& command, bool success);
