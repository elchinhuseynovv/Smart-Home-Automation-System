#include "display.h"

Display::Display() : display(128, 64, &Wire, -1) {
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

void Display::updateStatus(float temperature, float humidity, bool motion, int lightLevel) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    
    display.print("Temp: ");
    display.print(temperature);
    display.println(" C");
    
    display.print("Humidity: ");
    display.print(humidity);
    display.println("%");
    
    display.print("Motion: ");
    display.println(motion ? "YES" : "NO");
    
    display.print("Light: ");
    display.println(lightLevel);
    
    display.display();
}

void Display::clear() {
    display.clearDisplay();
    display.display();
}