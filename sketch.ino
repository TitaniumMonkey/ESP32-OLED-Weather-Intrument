#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_BMP085.h>
#include <DHT.h>
#include <ArduinoJson.h>
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

//DHT11 pin and type
#define DHTPIN 4        // GPIO4 (D6) (Adjust based on wiring)
#define DHTTYPE DHT11   // Define the sensor type as DHT11
DHT dht(DHTPIN, DHTTYPE);


// MQTT State Topics
const char* topic_state_temp = "homeassistant/sensor/esp32_temperature/state";
const char* topic_state_humidity = "homeassistant/sensor/esp32_humidity/state";
const char* topic_state_altitude = "homeassistant/sensor/esp32_altitude/state";
const char* topic_state_pressure = "homeassistant/sensor/esp32_pressure/state";

// MQTT Discovery Topics
const char* topic_discovery_temp = "homeassistant/sensor/esp32_temperature/config";
const char* topic_discovery_humidity = "homeassistant/sensor/esp32_humidity/config";
const char* topic_discovery_altitude = "homeassistant/sensor/esp32_altitude/config";
const char* topic_discovery_pressure = "homeassistant/sensor/esp32_pressure/config";

// Wi-Fi and MQTT Clients
WiFiClient espClient;
PubSubClient client(espClient);

// Time Configuration
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -8 * 3600;
const int daylightOffset_sec = 0;

unsigned long lastMQTTSentTime = 0;
unsigned long lastSerialPrintTime = 0;
const unsigned long serialInterval = 30000;

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

            // Always trigger discovery on startup
            bool discoveryNeeded = true;
            client.subscribe("homeassistant/status");

            client.setCallback([&discoveryNeeded](char* topic, byte* payload, unsigned int length) {
                if (strcmp(topic, "homeassistant/status") == 0 && strncmp((char*)payload, "online", length) == 0) {
                    Serial.println("✅ Home Assistant is online! Sending Auto Discovery.");
                    discoveryNeeded = true;
                }
            });

            client.loop();  // Process MQTT messages
            delay(1000);    // Short delay to allow HA response

            Serial.println("📡 Sending MQTT Auto Discovery...");

            StaticJsonDocument<256> discoveryDoc;
            char discoveryPayload[256];

            // Temperature Sensor (Fahrenheit)
            discoveryDoc.clear();
            discoveryDoc["name"] = "ESP32 Temperature";
            discoveryDoc["state_topic"] = topic_state_temp;
            discoveryDoc["unit_of_measurement"] = "°F";
            discoveryDoc["device_class"] = "temperature";
            discoveryDoc["value_template"] = "{{ value_json.temperature }}";
            serializeJson(discoveryDoc, discoveryPayload);
            client.publish(topic_discovery_temp, discoveryPayload, true);

            // Humidity Sensor
            discoveryDoc.clear();
            discoveryDoc["name"] = "ESP32 Humidity";
            discoveryDoc["state_topic"] = topic_state_humidity;
            discoveryDoc["unit_of_measurement"] = "%";
            discoveryDoc["device_class"] = "humidity";
            discoveryDoc["value_template"] = "{{ value_json.humidity }}";
            serializeJson(discoveryDoc, discoveryPayload);
            client.publish(topic_discovery_humidity, discoveryPayload, true);

            // Altitude Sensor
            discoveryDoc.clear();
            discoveryDoc["name"] = "ESP32 Altitude";
            discoveryDoc["state_topic"] = topic_state_altitude;
            discoveryDoc["unit_of_measurement"] = "ft";
            discoveryDoc["icon"] = "mdi:altimeter";
            discoveryDoc["value_template"] = "{{ value_json.altitude }}";
            serializeJson(discoveryDoc, discoveryPayload);
            client.publish(topic_discovery_altitude, discoveryPayload, true);

            // Pressure Sensor (hPa)
            discoveryDoc.clear();
            discoveryDoc["name"] = "ESP32 Pressure";
            discoveryDoc["state_topic"] = topic_state_pressure;
            discoveryDoc["unit_of_measurement"] = "hPa";
            discoveryDoc["device_class"] = "pressure";
            discoveryDoc["value_template"] = "{{ value_json.pressure_hPa }}";
            serializeJson(discoveryDoc, discoveryPayload);
            client.publish(topic_discovery_pressure, discoveryPayload, true);

            Serial.println("✅ MQTT Discovery Sent for 4 Sensors!");
        } else {
            Serial.println("❌ Failed to connect to MQTT Broker! Retrying in 5 seconds...");
            delay(5000);
        }
    }
}

void setup() {
    Serial.begin(19200);
    delay(500);  // Reduce startup delay
    Serial.println("\n🚀 ESP32 is starting up...");

    pinMode(BUTTON_PIN, INPUT_PULLUP);

    // Initialize BMP180 Sensor
    if (!bmp.begin()) {
        Serial.println("❌ BMP180 sensor not found! Restarting ESP...");
        delay(2000);
        ESP.restart();
    }

    // Initialize OLED Display
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("❌ SSD1306 OLED allocation failed! Restarting ESP...");
        delay(2000);
        ESP.restart();
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setRotation(2);

    // Initialize DHT11 Sensor
    dht.begin();
    delay(500);  // Shorter delay
    float testHumidity = dht.readHumidity();
    if (isnan(testHumidity)) {
        Serial.println("❌ DHT11 sensor not detected! Continuing...");
    } else {
        Serial.printf("✅ DHT11 initialized! First Humidity Reading: %.1f%%\n", testHumidity);
    }

    // Connect to Wi-Fi with timeout
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");

    int wifi_attempts = 0;
    while (WiFi.status() != WL_CONNECTED && wifi_attempts < 20) {  // Timeout after 20 attempts
        delay(500);
        Serial.print(".");
        wifi_attempts++;
        yield();  // Prevent watchdog reset
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n✅ Connected to Wi-Fi!");
    } else {
        Serial.println("\n❌ Failed to connect to Wi-Fi! Restarting ESP...");
        delay(2000);
        ESP.restart();
    }

    // Setup MQTT
    client.setServer(MQTT_SERVER, 1883);
    reconnectMQTT();

    // Configure NTP time synchronization
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    // Start OLED timer
    oledTimer = millis();

    Serial.println("✅ Setup completed! Entering main loop...");
}


void loop() {
    // Ensure MQTT connection is maintained
    if (!client.connected()) {
        Serial.println("MQTT disconnected. Reconnecting...");
        reconnectMQTT();
    }
    client.loop();

    // Handle button press for OLED toggle
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

    // **Temperature, Humidity, Altitude, and Pressure Readings**
    float temperatureC = bmp.readTemperature();
    float temperatureF = (temperatureC * 1.8) + 32;
    float humidity = dht.readHumidity();  

    const float PRESSURE_CALIBRATION_OFFSET = -2.92; // Adjust based on your reference
    float pressure = round((bmp.readPressure() / 100.0) + PRESSURE_CALIBRATION_OFFSET);

    // **Fix Altitude Readings**
    const float ALTITUDE_CALIBRATION_OFFSET = 21.0; // Adjust based on your reference
    float altitudeM = bmp.readAltitude() + ALTITUDE_CALIBRATION_OFFSET;
    float smoothedAltitudeM = round(getSmoothedAltitude(altitudeM));  
    float smoothedAltitudeF = round(smoothedAltitudeM * 3.28084);


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

    // **Get Current Time as String**
    time_t now = time(nullptr);
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);

    char timeStr[50];
    strftime(timeStr, sizeof(timeStr), "%m/%d/%y %I:%M%p PST", &timeinfo);

    // **Send MQTT message every 10 minutes with timestamp**
    static unsigned long mqttSentDisplayTime = 0;
    if (millis() - lastMQTTSentTime >= 120000) {  // 10 Minute delay
      StaticJsonDocument<200> jsonDoc;
      char mqttPayload[200];

      // Populate JSON document
      jsonDoc["timestamp"] = timeStr;
      jsonDoc["temperature_F"] = temperatureF;
      jsonDoc["humidity"] = humidity;
      jsonDoc["altitude"] = round(smoothedAltitudeF);  // Rounded to whole feet
      jsonDoc["pressure_hPa"] = pressure;

        // Publish Temperature in Fahrenheit
        StaticJsonDocument<50> tempFDoc;
        char tempFPayload[50];
        tempFDoc["temperature"] = temperatureF;
        serializeJson(tempFDoc, tempFPayload);
        client.publish(topic_state_temp, tempFPayload, true);

        // Publish Humidity (Rounded)
        StaticJsonDocument<50> humidityDoc;
        char humidityPayload[50];
        humidityDoc["humidity"] = round(humidity);
        serializeJson(humidityDoc, humidityPayload);
        client.publish(topic_state_humidity, humidityPayload, true);

        // Publish Altitude (Rounded)
        StaticJsonDocument<50> altitudeDoc;
        char altitudePayload[50];
        altitudeDoc["altitude"] = round(smoothedAltitudeF);
        serializeJson(altitudeDoc, altitudePayload);
        client.publish(topic_state_altitude, altitudePayload, true);


        // Publish Pressure in hPa
        StaticJsonDocument<50> pressureDoc;
        char pressurePayload[50];
        pressureDoc["pressure_hPa"] = pressure;
        serializeJson(pressureDoc, pressurePayload);
        client.publish(topic_state_pressure, pressurePayload, true);

        Serial.println("📡 MQTT messages sent!");

      // Display "MQTT Sent!" for 5 seconds
      mqttSentDisplayTime = millis();
    
      lastMQTTSentTime = millis();
}

      // **Serial Output (Every 30 Seconds)**
if (millis() - lastSerialPrintTime >= serialInterval) {
    lastSerialPrintTime = millis();  // Update last print time

    // **Read Humidity Only When Needed**
    float humidity = dht.readHumidity();
    if (isnan(humidity)) {
        humidity = -1;  // Use -1 to indicate a failed read
        Serial.println("⚠️ Failed to read from DHT sensor!");
    }

    // **Print Serial Output in Correct Order**
    Serial.printf("%s | Temp: %.2f C / %.2f F | Humidity: %.1f%% | Alt: %.0f m / %.0f ft | Pressure: %.0f hPa\n",
                  timeStr, temperatureC, temperatureF, humidity, smoothedAltitudeM, smoothedAltitudeF, pressure);
}

    // **OLED Display Update (Auto Shutoff Integrated)**
    if (oledOn) {
        display.clearDisplay();
        display.setTextColor(WHITE);
        display.setTextSize(1);

        // Temperature
        display.setCursor(0, 0);
        display.printf("Temp: %.1f C / %.1f F\n", temperatureC, temperatureF);

        // Humidity
        display.setCursor(0, 10);
        display.printf("Humidity: %.1f%%\n", humidity);

        // Altitude
        display.setCursor(0, 20);
        display.printf("Alt: %.0f m / %.0f ft\n", avgAltitudeOLED, avgAltitudeOLED * 3.28084);

        // Pressure
        display.setCursor(0, 30);
        display.printf("Pressure: %.0f hPa\n", avgPressureOLED);

        // Display "MQTT Sent!" for 5 seconds
        if (millis() - mqttSentDisplayTime <= 5000) {
            display.setCursor(0, 40);  
            display.println("MQTT Sent!");
        }

        // Time display (always at the bottom)
        display.setCursor(0, 54);
        display.println(timeStr);

        display.display();
    }

        // Display "MQTT Sent!" for 5 seconds
        if (millis() - mqttSentDisplayTime <= 5000) {
            display.setCursor(0, 40);  // Set position for the message
            display.println("MQTT Sent!");
        }

        display.setCursor(0, 54);
        display.println(timeStr);
        display.display();
    }
