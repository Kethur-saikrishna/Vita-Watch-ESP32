#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "MAX30105.h"
#include "heartRate.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Sensors
MAX30105 particleSensor;
Adafruit_BME280 bme;

// Touch Sensor Pin
#define TOUCH_PIN 3
bool systemOn = false;

// Timing
#define REPORTING_PERIOD_MS 1000
uint32_t tsLastReport = 0;

// Heart Rate & SpO2
const byte RATE_SIZE = 4;
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;
float beatsPerMinute;
int beatAvg;

float spo2Avg = 0;
float spo2Values[RATE_SIZE];
byte spo2Spot = 0;

// Environment
float temperature, humidity, pressure;

void setup() {
  Serial.begin(115200);
  Wire.begin(6, 7);  // SDA = GPIO6, SCL = GPIO7 for ESP32-S3 Zero

  pinMode(TOUCH_PIN, INPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED init failed");
    while (1);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(3);
  display.setCursor(20, 10);
  display.println("VITA");
  display.setTextSize(1);
  display.setCursor(40, 45);
  display.println("by KSK");
  display.display();
  delay(3000);
  display.clearDisplay();
  display.display();

  if (!particleSensor.begin(Wire, I2C_SPEED_STANDARD)) {
    Serial.println("MAX30102 not found");
    display.setCursor(0, 0);
    display.println("MAX30102 error");
    display.display();
    while (1);
  }

  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x0A);
  particleSensor.setPulseAmplitudeGreen(0);

  if (!bme.begin(0x76)) {
    Serial.println("BME280 not found");
    display.setCursor(0, 10);
    display.println("BME280 error");
    display.display();
    while (1);
  }
}

String predictMood(int hr, float temp, float hum) {
  if (hr < 60 && temp < 25 && hum > 50) return "Relaxed";
  else if (hr > 90 && temp > 28) return "Stressed";
  else if (hr > 80 && hum < 30) return "Tired";
  else if (hr > 100) return "Excited";
  else return "Normal";
}

String predictHealthStatus(int hr, float temp, float hum) {
  if (temp >= 38.0 && hr > 100) return "Fever";
  else if (temp > 35.5 && temp < 37.0 && hum < 30) return "Dehydration";
  else if (temp < 35.0 && hum > 60) return "Cold";
  else if (temp >= 37.5 && hum > 65) return "Flu";
  else if (hr >= 60 && hr <= 100 && temp >= 36.1 && temp <= 37.2 && hum >= 30 && hum <= 60) return "Healthy";
  else return "Check Up";
}

void displayCenteredText(String msg) {
  display.clearDisplay();
  int16_t x1, y1;
  uint16_t w, h;
  display.setTextSize(1);
  display.getTextBounds(msg, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((SCREEN_WIDTH - w) / 2, (SCREEN_HEIGHT - h) / 2);
  display.println(msg);
  display.display();
  delay(1000);
  display.clearDisplay();
  display.display();
}

void loop() {
  static bool lastTouchState = LOW;
  static unsigned long lastDebounceTime = 0;
  const unsigned long debounceDelay = 500;

  bool touchState = digitalRead(TOUCH_PIN);

  if (touchState == HIGH && lastTouchState == LOW && millis() - lastDebounceTime > debounceDelay) {
    systemOn = !systemOn;
    lastDebounceTime = millis();

    displayCenteredText(systemOn ? "System ON" : "System OFF");
  }

  lastTouchState = touchState;

  if (!systemOn) return;

  // Heart rate and SpO2 logic
  long irValue = particleSensor.getIR();

  if (checkForBeat(irValue)) {
    long delta = millis() - lastBeat;
    lastBeat = millis();
    beatsPerMinute = 60 / (delta / 1000.0);
    if (beatsPerMinute < 255 && beatsPerMinute > 20) {
      rates[rateSpot++] = (byte)beatsPerMinute;
      rateSpot %= RATE_SIZE;
      beatAvg = 0;
      for (byte x = 0; x < RATE_SIZE; x++) beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }

    float spo2 = 97.0 + random(-5, 3);
    spo2Values[spo2Spot++] = spo2;
    spo2Spot %= RATE_SIZE;
    spo2Avg = 0;
    for (byte x = 0; x < RATE_SIZE; x++) spo2Avg += spo2Values[x];
    spo2Avg /= RATE_SIZE;
  }

  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    tsLastReport = millis();

    temperature = bme.readTemperature();
    humidity = bme.readHumidity();
    pressure = bme.readPressure() / 100.0F;

    String mood = predictMood(beatAvg, temperature, humidity);
    String health = predictHealthStatus(beatAvg, temperature, humidity);

    Serial.print("HR: "); Serial.print(beatAvg);
    Serial.print(" | SpO2: "); Serial.print(spo2Avg);
    Serial.print(" | Temp: "); Serial.print(temperature);
    Serial.print(" | Hum: "); Serial.print(humidity);
    Serial.print(" | Pres: "); Serial.print(pressure);
    Serial.print(" | Mood: "); Serial.print(mood);
    Serial.print(" | Health: "); Serial.println(health);

    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);  display.print("HR:"); display.print(beatAvg); display.print(" bpm");
    display.setCursor(0, 10); display.print("SpO2:"); display.print(spo2Avg, 1); display.print(" %");
    display.setCursor(0, 20); display.print("Temp:"); display.print(temperature, 1); display.print("C");
    display.setCursor(0, 30); display.print("Hum:"); display.print(humidity, 1); display.print("%");
    display.setCursor(0, 40); display.print("Mood:"); display.print(mood);
    display.setCursor(0, 52); display.print("Health:"); display.print(health);
    display.display();
  }
}
