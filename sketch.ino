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
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_BMP085 bmp;

// Button Configuration
#define BUTTON_PIN 0  // ESP32 Built-in button (NOT the reset button)
bool oledOn = true;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 200;

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
const unsigned long mqttInterval = 300000; // 5 minutes
unsigned long lastSerialPrintTime = 0;
const unsigned long serialInterval = 15000; // 15 seconds

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

void waitForTimeSync() {
    Serial.print("Synchronizing time");
    time_t now = time(nullptr);
    int retries = 10;

    while (now < 1700000000 && retries > 0) {
        Serial.print(".");
        delay(1000);
        now = time(nullptr);
        retries--;
    }

    if (now < 1700000000) {
        Serial.println("\n⚠️ Time sync failed! Check your internet connection or NTP server.");
    } else {
        Serial.println("\n⏳ Time synchronized successfully!");
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

    // Connect to Wi-Fi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\n✅ Connected to Wi-Fi!");

    // Connect to MQTT Broker
    client.setServer(MQTT_SERVER, 1883);
    reconnectMQTT();

    // Configure and synchronize time
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    waitForTimeSync();
}

void loop() {
    if (!client.connected()) {
        Serial.println("MQTT disconnected. Reconnecting...");
        reconnectMQTT();
    }
    client.loop();

    // Handle OLED button toggle
    static bool buttonPressed = false;
    if (digitalRead(BUTTON_PIN) == LOW) {
        if (!buttonPressed && (millis() - lastDebounceTime > debounceDelay)) {
            buttonPressed = true;
            lastDebounceTime = millis();
            oledOn = !oledOn;
            Serial.printf("📟 OLED %s\n", oledOn ? "ON" : "OFF");

            if (!oledOn) {
                display.clearDisplay();
                display.display();
            }
        }
    } else {
        buttonPressed = false;
    }

    // Read temperature, altitude, and pressure
    float temperatureC = bmp.readTemperature();
    float temperatureF = (temperatureC * 1.8) + 32;
    float pressure = bmp.readPressure() / 100.0; // Convert Pa to hPa
    float altitudeM = bmp.readAltitude(); // Get altitude in meters
    float altitudeF = altitudeM * 3.28084; // Convert altitude to feet

    if (isnan(temperatureC) || isnan(pressure) || isnan(altitudeM)) {
        Serial.println("⚠️ Failed to read from BMP180 sensor!");
        return;
    }

    // Serial output every 15 seconds
    if (millis() - lastSerialPrintTime >= serialInterval) {
        time_t now = time(nullptr);
        struct tm timeinfo;
        localtime_r(&now, &timeinfo);
        
        char timeStr[50];
        strftime(timeStr, sizeof(timeStr), "%m/%d/%y %I:%M%p PST", &timeinfo);

        Serial.printf("%s | Temp: %.2f C / %.2f F | Alt: %.2f m / %.0f ft | Pressure: %.2f hPa\n",
                      timeStr, temperatureC, temperatureF, altitudeM, altitudeF, pressure);
        lastSerialPrintTime = millis();
    }

    // Send MQTT message every 5 minutes
    if (millis() - lastMQTTSentTime >= mqttInterval) {
        char message[100];
        snprintf(message, sizeof(message), "Temp: %.2f C / %.2f F | Alt: %.2f m / %.0f ft | Pressure: %.2f hPa",
                 temperatureC, temperatureF, altitudeM, altitudeF, pressure);
        client.publish(topic, message, true);
        lastMQTTSentTime = millis();
    }

    // Update OLED display only if it's ON
    if (oledOn) {
        display.clearDisplay();
        display.setTextColor(WHITE);
        
        // Display Temperature First
        display.setTextSize(1);
        display.setCursor(0, 0);
        display.print("Temp: ");
        display.print(temperatureC, 1);
        display.print(" C / ");
        display.print(temperatureF, 1);
        display.println(" F");

        // Display Altitude Next
        display.setCursor(0, 10);
        display.print("Alt: ");
        display.print(altitudeM, 1);
        display.print(" m / ");
        display.print(altitudeF, 0);
        display.println(" ft");

        // Display Pressure Last
        display.setCursor(0, 20);
        display.print("Pressure: ");
        display.print(pressure, 1);
        display.println(" hPa");

        // Display Time
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
