#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

class Display {
public:
    Display();
    bool begin();
    void updateStatus(float temperature, float humidity, bool motion, float lightLevel, bool isRaining, float airQuality);
    void showAlert(const char* message);
    void showGraph(float data[], int count, const char* title);
    void clear();
    void setBrightness(uint8_t brightness);
    void toggleDisplayMode();
    
private:
    Adafruit_SSD1306 display;
    bool detailedMode;
    void drawProgressBar(int x, int y, int width, int height, int progress);
    void displayBasicInfo(float temperature, float humidity, bool motion, float lightLevel);
    void displayDetailedInfo(float temperature, float humidity, bool motion, float lightLevel, bool isRaining, float airQuality);
};