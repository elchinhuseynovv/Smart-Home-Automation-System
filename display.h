#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

enum DisplayPage {
    MAIN,
    ENVIRONMENT,
    SECURITY,
    ENERGY,
    SETTINGS
};

class Display {
public:
    Display();
    bool begin();
    
    // Enhanced display methods
    void updateStatus(float temperature, float humidity, bool motion, float lightLevel, bool isRaining, float airQuality);
    void showAlert(const char* message);
    void showGraph(float data[], int count, const char* title);
    void showEnergyStats(float consumption, float solar, float battery);
    void showSecurityStatus(bool doorLocked, bool windowsClosed, bool motionDetected);
    void showWeatherForecast(float tempTrend, float humidityTrend, float pressureTrend);
    
    // Display control
    void clear();
    void setBrightness(uint8_t brightness);
    void toggleDisplayMode();
    void nextPage();
    void previousPage();
    void setAutoPageChange(bool enabled, unsigned long interval);
    
private:
    Adafruit_SSD1306 display;
    bool detailedMode;
    DisplayPage currentPage;
    bool autoPageChange;
    unsigned long pageChangeInterval;
    unsigned long lastPageChange;
    
    // Helper methods
    void drawProgressBar(int x, int y, int width, int height, int progress);
    void displayBasicInfo(float temperature, float humidity, bool motion, float lightLevel);
    void displayDetailedInfo(float temperature, float humidity, bool motion, float lightLevel, bool isRaining, float airQuality);
    void displayEnvironmentPage(float temperature, float humidity, float pressure, float airQuality);
    void displaySecurityPage(bool doorLocked, bool windowsClosed, bool motionDetected);
    void displayEnergyPage(float consumption, float solar, float battery);
    void displaySettingsPage();
    void drawTrendIndicator(int x, int y, float trend);
};

#endif