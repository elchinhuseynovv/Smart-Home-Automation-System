/*
   Advanced Smart Home Automation System
   - Controls lights with motion detection
   - Monitors temperature and triggers fan
   - Displays status on an OLED screen
*/

#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Servo.h>

// Pin Definitions
#define DHTPIN 2
#define DHTTYPE DHT11
#define PIRPIN 3
#define LEDPIN 4
#define FANPIN 5
#define SERVO_PIN 6

// Sensor and Display Setup
DHT dht(DHTPIN, DHTTYPE);
Adafruit_SSD1306 display(128, 64, &Wire, -1);
Servo doorServo;

void setup() {
    Serial.begin(9600);
    dht.begin();
    pinMode(PIRPIN, INPUT);
    pinMode(LEDPIN, OUTPUT);
    pinMode(FANPIN, OUTPUT);
    doorServo.attach(SERVO_PIN);
    doorServo.write(0);

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;);
    }
    display.display();
    delay(2000);
    display.clearDisplay();
}

void loop() {
    float temp = dht.readTemperature();
    int motion = digitalRead(PIRPIN);

    // Motion-activated lights
    if (motion == HIGH) {
        digitalWrite(LEDPIN, HIGH);
        Serial.println("Motion detected: Lights ON");
    } else {
        digitalWrite(LEDPIN, LOW);
    }

    // Temperature-controlled fan
    if (temp > 30.0) {
        digitalWrite(FANPIN, HIGH);
        Serial.println("Fan ON: High temperature detected");
    } else {
        digitalWrite(FANPIN, LOW);
    }

    // Display status on OLED
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.print("Temp: ");
    display.print(temp);
    display.println(" C");
    display.print("Motion: ");
    display.println(motion ? "YES" : "NO");
    display.display();

    // Simulate smart door unlock with servo
    if (motion == HIGH && temp < 28.0) {
        doorServo.write(90);
        delay(5000);
        doorServo.write(0);
    }

    delay(2000);
}
