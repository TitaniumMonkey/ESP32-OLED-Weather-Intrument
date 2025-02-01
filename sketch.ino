#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_BMP085.h>
#include <time.h>
#include "secrets.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_BMP085 bmp;

// Button Configuration
#define BUTTON_PIN 0  
bool oledOn = true;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 200;
unsigned long oledTimer = 0;
const unsigned long oledTimeout = 300000;  // 5 minutes (300,000 ms)

// MQTT topic
const char* topic = "Sensors/BMP180/01";

// Wi-Fi and MQTT Clients
WiFiClient espClient;
PubSubClient client(espClient);

// Time Configuration
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -8 * 3600;
const int daylightOffset_sec = 0;

unsigned long lastMQTTSentTime = 0;
const unsigned long mqttInterval = 300000;
unsigned long lastSerialPrintTime = 0;
const unsigned long serialInterval = 15000;

// **Moving Average Filter for Altitude**
#define ALTITUDE_SAMPLES 5
float altitudeBuffer[ALTITUDE_SAMPLES] = {0};
unsigned long altitudeTimeBuffer[ALTITUDE_SAMPLES] = {0};
int altitudeIndex = 0;
bool bufferFilled = false;

float getSmoothedAltitude(float newAltitude) {
    altitudeBuffer[altitudeIndex] = newAltitude;
    altitudeTimeBuffer[altitudeIndex] = millis();
    altitudeIndex = (altitudeIndex + 1) % ALTITUDE_SAMPLES;

    if (!bufferFilled && altitudeIndex == 0) {
        bufferFilled = true;
    }

    float sum = 0;
    int count = 0;
    unsigned long currentTime = millis();
    
    for (int i = 0; i < ALTITUDE_SAMPLES; i++) {
        if (currentTime - altitudeTimeBuffer[i] <= 5000) {  
            sum += altitudeBuffer[i];
            count++;
        }
    }
    
    return count > 0 ? sum / count : newAltitude;
}

// **Averaging for OLED display**
float avgAltitudeOLED = 0;
float avgPressureOLED = 0;
float altSum = 0;
float pressSum = 0;
int sampleCount = 0;

void reconnectMQTT() {
    while (!client.connected()) {
        if (client.connect("ESP32Weather", MQTT_USER, MQTT_PASS)) {
            Serial.println("✅ Connected to MQTT Broker!");
        } else {
            Serial.println("❌ Failed to connect to MQTT Broker! Retrying in 5 seconds...");
            delay(5000);
        }
    }
}

void setup() {
    Serial.begin(19200);
    delay(2000);
    Serial.println("\n🚀 ESP32 is starting up...");

    pinMode(BUTTON_PIN, INPUT_PULLUP);

    if (!bmp.begin()) {
        Serial.println("❌ BMP180 sensor not found!");
        while (1);
    }

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("SSD1306 allocation failed");
        for (;;);
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setRotation(2);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\n✅ Connected to Wi-Fi!");

    client.setServer(MQTT_SERVER, 1883);
    reconnectMQTT();

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    oledTimer = millis();  // Start the OLED timer
}

void loop() {
    if (!client.connected()) {
        Serial.println("MQTT disconnected. Reconnecting...");
        reconnectMQTT();
    }
    client.loop();

    static bool buttonPressed = false;
    if (digitalRead(BUTTON_PIN) == LOW) {
        if (!buttonPressed && (millis() - lastDebounceTime > debounceDelay)) {
            buttonPressed = true;
            lastDebounceTime = millis();
            oledOn = !oledOn;
            Serial.printf("📟 OLED %s\n", oledOn ? "ON" : "OFF");

            if (oledOn) {
                oledTimer = millis();  // Reset OLED timer when turned on
            } else {
                display.clearDisplay();
                display.display();
            }
        }
    } else {
        buttonPressed = false;
    }

    // **Auto Shutoff OLED After 5 Minutes**
    if (oledOn && (millis() - oledTimer >= oledTimeout)) {
        oledOn = false;
        Serial.println("⏳ OLED turned off automatically after 5 minutes.");
        display.clearDisplay();
        display.display();
    }

    // **Temperature, Altitude, and Pressure Readings**
    float temperatureC = bmp.readTemperature();
    float temperatureF = (temperatureC * 1.8) + 32;  
    const float PRESSURE_CALIBRATION_OFFSET = -2.92; // Adjust based on your reference
    float pressure = round((bmp.readPressure() / 100.0) + PRESSURE_CALIBRATION_OFFSET);


    // **Fix Altitude Readings**
    const float ALTITUDE_CALIBRATION_OFFSET = 34.4; // Adjust this based on your reference
    float altitudeM = bmp.readAltitude() + ALTITUDE_CALIBRATION_OFFSET;
    float smoothedAltitudeM = getSmoothedAltitude(altitudeM);
    float smoothedAltitudeF = smoothedAltitudeM * 3.28084;

    // **Averaging Altitude & Pressure**
    static unsigned long lastSampleTime = 0;
    if (millis() - lastSampleTime >= 1000) {
        lastSampleTime = millis();
        altSum += smoothedAltitudeM;
        pressSum += pressure;
        sampleCount++;
    }

    static unsigned long lastAvgUpdateTime = 0;
    if (millis() - lastAvgUpdateTime >= 5000 && sampleCount > 0) {
        avgAltitudeOLED = altSum / sampleCount;
        avgPressureOLED = pressSum / sampleCount;
        altSum = 0;
        pressSum = 0;
        sampleCount = 0;
        lastAvgUpdateTime = millis();
    }

    // **Serial Output**
    if (millis() - lastSerialPrintTime >= serialInterval) {
        time_t now = time(nullptr);
        struct tm timeinfo;
        localtime_r(&now, &timeinfo);
        
        char timeStr[50];
        strftime(timeStr, sizeof(timeStr), "%m/%d/%y %I:%M%p PST", &timeinfo);

        Serial.printf("%s | Temp: %.2f C / %.2f F | Alt: %.1f m / %.0f ft | Pressure: %.0f hPa\n",
                      timeStr, temperatureC, temperatureF, smoothedAltitudeM, smoothedAltitudeF, pressure);
        lastSerialPrintTime = millis();
    }

    // **OLED Display (Auto Shutoff Integrated)**
    if (oledOn) {
        display.clearDisplay();
        display.setTextColor(WHITE);
        display.setTextSize(1);
        display.setCursor(0, 0);
        display.printf("Temp: %.1f C / %.1f F\n", temperatureC, temperatureF);
        display.setCursor(0, 10);
        display.printf("Alt: %.1f m / %.0f ft\n", avgAltitudeOLED, avgAltitudeOLED * 3.28084);
        display.setCursor(0, 20);
        display.printf("Pressure: %.0f hPa\n", avgPressureOLED);

        time_t now = time(nullptr);
        struct tm timeinfo;
        localtime_r(&now, &timeinfo);
        char timeStr[50];
        strftime(timeStr, sizeof(timeStr), "%m/%d/%y %I:%M%p PST", &timeinfo);

        display.setCursor(0, 54);
        display.println(timeStr);
        display.display();
    }
}
