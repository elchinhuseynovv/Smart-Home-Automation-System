#include "voice_control.h"

VoiceControl::VoiceControl()
    : isListening(false), confidenceThreshold(0.85), lastCommand("") {
}

void VoiceControl::begin() {
    calibrateMicrophone();
}

void VoiceControl::processAudioInput() {
    // Simulated audio processing
    String audioData = "increase temperature to 24";
    VoiceCommand cmd = recognizeCommand(audioData);
    
    if (cmd != NONE_CMD) {
        String params = extractParameters(audioData);
        executeCommand(cmd, params);
    }
}

VoiceCommand VoiceControl::recognizeCommand(const String& audioData) {
    // Simple command recognition logic
    if (audioData.indexOf("lights on") != -1) return LIGHTS_ON;
    if (audioData.indexOf("lights off") != -1) return LIGHTS_OFF;
    if (audioData.indexOf("temperature") != -1) return SET_TEMPERATURE;
    if (audioData.indexOf("open windows") != -1) return OPEN_WINDOWS;
    if (audioData.indexOf("close windows") != -1) return CLOSE_WINDOWS;
    if (audioData.indexOf("security status") != -1) return SECURITY_STATUS;
    if (audioData.indexOf("energy report") != -1) return ENERGY_REPORT;
    if (audioData.indexOf("weather") != -1) return WEATHER_REPORT;
    
    return NONE_CMD;
}

void VoiceControl::executeCommand(VoiceCommand cmd, const String& parameters) {
    switch (cmd) {
        case LIGHTS_ON:
            actuators.setLight(255);
            speak("Lights turned on");
            break;
            
        case LIGHTS_OFF:
            actuators.setLight(0);
            speak("Lights turned off");
            break;
            
        case SET_TEMPERATURE:
            if (parameters.length() > 0) {
                float temp = parameters.toFloat();
                automation.setTargetTemperature(temp);
                speak("Temperature set to " + String(temp) + " degrees");
            }
            break;
            
        case OPEN_WINDOWS:
            actuators.setWindowOpening(100);
            speak("Opening windows");
            break;
            
        case CLOSE_WINDOWS:
            actuators.setWindowOpening(0);
            speak("Closing windows");
            break;
            
        case SECURITY_STATUS:
            String status = automation.getSecurityStatus();
            speak("Security status: " + status);
            break;
    }
    
    logVoiceActivity(String(cmd), true);
}

void VoiceControl::speak(const String& message) {
    // Simulated voice synthesis
    Serial.println("Voice: " + message);
}

void VoiceControl::trainNewCommand(const String& command, const String& action) {
    updateCommandDatabase(command, action);
    speak("New command learned: " + command);
}

void VoiceControl::calibrateMicrophone() {
    // Microphone calibration logic
    isListening = true;
}

String VoiceControl::extractParameters(const String& audioInput) {
    // Simple parameter extraction
    int toIndex = audioInput.indexOf(" to ");
    if (toIndex != -1) {
        return audioInput.substring(toIndex + 4);
    }
    return "";
}

void VoiceControl::logVoiceActivity(const String& command, bool success) {
    // Log voice commands for analysis
    lastCommand = command;
}

void VoiceControl::updateCommandDatabase(const String& command, const String& pattern) {
    // Update voice command patterns
}