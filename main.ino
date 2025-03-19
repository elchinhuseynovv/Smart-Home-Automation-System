/*
   Advanced Smart Home Automation System
   - Controls lights with motion detection
   - Monitors temperature and triggers fan
   - Displays status on an OLED screen
   - Unlocks door based on motion and temperature
   - Monitors humidity and alerts if too high
   - Sends an alert if motion is detected at night
   - Controls brightness of LED based on ambient light
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
#define LDRPIN A0
#define BUZZERPIN 7

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
    pinMode(LDRPIN, INPUT);
    pinMode(BUZZERPIN, OUTPUT);
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
    float humidity = dht.readHumidity();
    int motion = digitalRead(PIRPIN);
    int lightLevel = analogRead(LDRPIN);

    // Motion-activated lights with brightness control
    if (motion == HIGH) {
        int brightness = map(lightLevel, 0, 1023, 255, 0);
        analogWrite(LEDPIN, brightness);
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

    // Humidity monitoring
    if (humidity > 70.0) {
        Serial.println("Warning: High humidity detected!");
    }

    // Night-time motion alert (Assume darkness if lightLevel is low)
    if (motion == HIGH && lightLevel < 300) {
        digitalWrite(BUZZERPIN, HIGH);
        Serial.println("ALERT: Motion detected at night!");
        delay(1000);
        digitalWrite(BUZZERPIN, LOW);
    }

    // Display status on OLED
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.print("Temp: ");
    display.print(temp);
    display.println(" C");
    display.print("Humidity: ");
    display.print(humidity);
    display.println("%");
    display.print("Motion: ");
    display.println(motion ? "YES" : "NO");
    display.print("Light: ");
    display.println(lightLevel);
    display.display();

    // Smart Door Unlock
    if (motion == HIGH && temp < 28.0) {
        Serial.println("Unlocking door...");
        doorServo.write(90);
        delay(5000);
        doorServo.write(0);
        Serial.println("Door locked");
    }

    delay(2000);
}
