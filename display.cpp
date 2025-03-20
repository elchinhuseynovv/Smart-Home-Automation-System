#include "display.h"

Display::Display() 
    : display(128, 64, &Wire, -1), 
      detailedMode(false),
      currentPage(MAIN),
      autoPageChange(false),
      pageChangeInterval(5000),
      lastPageChange(0) {
}

bool Display::begin() {
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        return false;
    }
    display.display();
    delay(2000);
    display.clearDisplay();
    return true;
}

void Display::updateStatus(float temperature, float humidity, bool motion, float lightLevel, bool isRaining, float airQuality) {
    display.clearDisplay();
    
    switch (currentPage) {
        case MAIN:
            if (detailedMode) {
                displayDetailedInfo(temperature, humidity, motion, lightLevel, isRaining, airQuality);
            } else {
                displayBasicInfo(temperature, humidity, motion, lightLevel);
            }
            break;
            
        case ENVIRONMENT:
            displayEnvironmentPage(temperature, humidity, 0, airQuality);
            break;
            
        case SECURITY:
            displaySecurityPage(true, !isRaining, motion);
            break;
            
        case ENERGY:
            displayEnergyPage(0, 0, 0);
            break;
            
        case SETTINGS:
            displaySettingsPage();
            break;
    }
    
    display.display();
    
    // Handle auto page change
    if (autoPageChange && (millis() - lastPageChange >= pageChangeInterval)) {
        nextPage();
        lastPageChange = millis();
    }
}

void Display::showEnergyStats(float consumption, float solar, float battery) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    
    // Energy consumption
    display.setCursor(0, 0);
    display.println("Energy Usage:");
    drawProgressBar(0, 10, 128, 8, map(consumption, 0, 5000, 0, 100));
    display.setCursor(0, 20);
    display.print(consumption);
    display.println(" W");
    
    // Solar production
    display.setCursor(0, 30);
    display.println("Solar:");
    drawProgressBar(0, 40, 128, 8, map(solar, 0, 2000, 0, 100));
    display.setCursor(0, 50);
    display.print(solar);
    display.println(" W");
    
    // Battery level
    display.setCursor(64, 30);
    display.println("Battery:");
    drawProgressBar(64, 40, 64, 8, battery);
    display.setCursor(64, 50);
    display.print(battery);
    display.println("%");
    
    display.display();
}

void Display::showSecurityStatus(bool doorLocked, bool windowsClosed, bool motionDetected) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    
    display.setCursor(0, 0);
    display.println("Security Status");
    
    display.setCursor(0, 16);
    display.print("Door: ");
    display.println(doorLocked ? "LOCKED" : "UNLOCKED");
    
    display.setCursor(0, 26);
    display.print("Windows: ");
    display.println(windowsClosed ? "CLOSED" : "OPEN");
    
    display.setCursor(0, 36);
    display.print("Motion: ");
    display.println(motionDetected ? "DETECTED" : "NONE");
    
    display.display();
}

void Display::showWeatherForecast(float tempTrend, float humidityTrend, float pressureTrend) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    
    display.setCursor(0, 0);
    display.println("Weather Forecast");
    
    // Temperature trend
    display.setCursor(0, 16);
    display.print("Temp: ");
    drawTrendIndicator(50, 16, tempTrend);
    
    // Humidity trend
    display.setCursor(0, 26);
    display.print("Humidity: ");
    drawTrendIndicator(50, 26, humidityTrend);
    
    // Pressure trend
    display.setCursor(0, 36);
    display.print("Pressure: ");
    drawTrendIndicator(50, 36, pressureTrend);
    
    display.display();
}

void Display::nextPage() {
    currentPage = static_cast<DisplayPage>((currentPage + 1) % 5);
    lastPageChange = millis();
}

void Display::previousPage() {
    currentPage = static_cast<DisplayPage>((currentPage + 4) % 5);
    lastPageChange = millis();
}

void Display::setAutoPageChange(bool enabled, unsigned long interval) {
    autoPageChange = enabled;
    if (interval > 0) {
        pageChangeInterval = interval;
    }
}

void Display::drawTrendIndicator(int x, int y, float trend) {
    if (trend > 0.1) {
        display.fillTriangle(x, y+6, x+4, y, x+8, y+6, WHITE);
    } else if (trend < -0.1) {
        display.fillTriangle(x, y, x+4, y+6, x+8, y, WHITE);
    } else {
        display.drawFastHLine(x, y+3, 8, WHITE);
    }
}

void Display::displayEnvironmentPage(float temperature, float humidity, float pressure, float airQuality) {
    display.setTextSize(1);
    display.setTextColor(WHITE);
    
    display.setCursor(0, 0);
    display.println("Environment");
    
    // Temperature
    display.setCursor(0, 16);
    display.print("Temp: ");
    display.print(temperature, 1);
    display.println("C");
    drawProgressBar(64, 16, 64, 8, map(temperature, 0, 40, 0, 100));
    
    // Humidity
    display.setCursor(0, 32);
    display.print("Hum: ");
    display.print(humidity, 1);
    display.println("%");
    drawProgressBar(64, 32, 64, 8, humidity);
    
    // Air Quality
    display.setCursor(0, 48);
    display.print("Air: ");
    display.print(airQuality, 1);
    display.println("%");
    drawProgressBar(64, 48, 64, 8, airQuality);
}

void Display::displaySecurityPage(bool doorLocked, bool windowsClosed, bool motionDetected) {
    display.setTextSize(1);
    display.setTextColor(WHITE);
    
    display.setCursor(0, 0);
    display.println("Security Status");
    
    // Door status
    display.setCursor(0, 16);
    display.print("Door: ");
    display.println(doorLocked ? "Locked" : "Unlocked");
    
    // Windows status
    display.setCursor(0, 32);
    display.print("Windows: ");
    display.println(windowsClosed ? "Closed" : "Open");
    
    // Motion status
    display.setCursor(0, 48);
    display.print("Motion: ");
    display.println(motionDetected ? "Detected" : "None");
}

void Display::displayEnergyPage(float consumption, float solar, float battery) {
    display.setTextSize(1);
    display.setTextColor(WHITE);
    
    display.setCursor(0, 0);
    display.println("Energy Status");
    
    // Power consumption
    display.setCursor(0, 16);
    display.print("Usage: ");
    display.print(consumption);
    display.println("W");
    drawProgressBar(0, 24, 128, 8, map(consumption, 0, 5000, 0, 100));
    
    // Solar production
    display.setCursor(0, 36);
    display.print("Solar: ");
    display.print(solar);
    display.println("W");
    drawProgressBar(0, 44, 128, 8, map(solar, 0, 2000, 0, 100));
    
    // Battery level
    display.setCursor(0, 52);
    display.print("Batt: ");
    display.print(battery);
    display.println("%");
}

void Display::displaySettingsPage() {
    display.setTextSize(1);
    display.setTextColor(WHITE);
    
    display.setCursor(0, 0);
    display.println("Settings");
    
    display.setCursor(0, 16);
    display.print("Mode: ");
    display.println(detailedMode ? "Detailed" : "Basic");
    
    display.setCursor(0, 32);
    display.print("Auto Page: ");
    display.println(autoPageChange ? "ON" : "OFF");
    
    display.setCursor(0, 48);
    display.print("Interval: ");
    display.print(pageChangeInterval / 1000);
    display.println("s");
}