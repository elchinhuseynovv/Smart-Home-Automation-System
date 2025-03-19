#include "display.h"

Display::Display() : display(128, 64, &Wire, -1), detailedMode(false) {
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
    
    if (detailedMode) {
        displayDetailedInfo(temperature, humidity, motion, lightLevel, isRaining, airQuality);
    } else {
        displayBasicInfo(temperature, humidity, motion, lightLevel);
    }
    
    display.display();
}

void Display::displayBasicInfo(float temperature, float humidity, bool motion, float lightLevel) {
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    
    display.print("Temp: ");
    display.print(temperature, 1);
    display.println(" C");
    
    display.print("Humidity: ");
    display.print(humidity, 1);
    display.println("%");
    
    display.print("Motion: ");
    display.println(motion ? "YES" : "NO");
    
    display.print("Light: ");
    display.print(lightLevel, 1);
    display.println(" lux");
}

void Display::displayDetailedInfo(float temperature, float humidity, bool motion, float lightLevel, bool isRaining, float airQuality) {
    display.setTextSize(1);
    display.setTextColor(WHITE);
    
    // First column
    display.setCursor(0, 0);
    display.println("Environment:");
    drawProgressBar(0, 12, 60, 8, temperature * 2);
    display.setCursor(0, 22);
    display.print(temperature, 1);
    display.println("C");
    
    // Second column
    display.setCursor(64, 0);
    display.println("Humidity:");
    drawProgressBar(64, 12, 60, 8, humidity);
    display.setCursor(64, 22);
    display.print(humidity, 1);
    display.println("%");
    
    // Status indicators
    display.setCursor(0, 32);
    display.print("Motion: ");
    display.println(motion ? "Active" : "None");
    
    display.print("Rain: ");
    display.println(isRaining ? "Yes" : "No");
    
    // Air quality bar
    display.setCursor(0, 52);
    display.print("Air: ");
    drawProgressBar(25, 54, 103, 8, airQuality);
}

void Display::showAlert(const char* message) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("ALERT!");
    display.setTextSize(1);
    display.println(message);
    display.display();
}

void Display::showGraph(float data[], int count, const char* title) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    
    // Draw title
    display.setCursor(0, 0);
    display.println(title);
    
    // Find min and max for scaling
    float minVal = data[0];
    float maxVal = data[0];
    for (int i = 1; i < count; i++) {
        if (data[i] < minVal) minVal = data[i];
        if (data[i] > maxVal) maxVal = data[i];
    }
    
    // Draw graph
    int graphHeight = 40;
    int graphBottom = 63;
    for (int i = 0; i < count - 1; i++) {
        int x1 = map(i, 0, count - 1, 0, 127);
        int x2 = map(i + 1, 0, count - 1, 0, 127);
        int y1 = map(data[i], minVal, maxVal, graphBottom, graphBottom - graphHeight);
        int y2 = map(data[i + 1], minVal, maxVal, graphBottom, graphBottom - graphHeight);
        display.drawLine(x1, y1, x2, y2, WHITE);
    }
    
    display.display();
}

void Display::drawProgressBar(int x, int y, int width, int height, int progress) {
    progress = constrain(progress, 0, 100);
    display.drawRect(x, y, width, height, WHITE);
    display.fillRect(x + 1, y + 1, (width - 2) * progress / 100, height - 2, WHITE);
}

void Display::clear() {
    display.clearDisplay();
    display.display();
}

void Display::setBrightness(uint8_t brightness) {
    display.ssd1306_command(SSD1306_SETCONTRAST);
    display.ssd1306_command(brightness);
}

void Display::toggleDisplayMode() {
    detailedMode = !detailedMode;
}