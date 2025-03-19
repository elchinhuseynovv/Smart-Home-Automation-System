#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

class Display {
public:
    Display();
    bool begin();
    void updateStatus(float temperature, float humidity, bool motion, int lightLevel);
    void clear();
    
private:
    Adafruit_SSD1306 display;
};