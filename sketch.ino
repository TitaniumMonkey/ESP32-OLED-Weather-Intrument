#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <time.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "secrets.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// MQTT topic
const char* topic = "Sensors/DHT11/01";

// DHT Sensor Setup
#define DHTPIN 15
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Wi-Fi and MQTT Clients
WiFiClient espClient;
PubSubClient client(espClient);

// Time Configuration
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -8 * 3600; // Pacific Standard Time (PST)
const int daylightOffset_sec = 0;     // No DST

unsigned long lastMQTTSentTime = 0;
const unsigned long mqttInterval = 300000; // 5 minutes in milliseconds

unsigned long lastSerialPrintTime = 0;
const unsigned long serialInterval = 5000; // 5 seconds in milliseconds

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
    Serial.println("\n⚠️  Time sync failed! Check your internet connection or NTP server.");
  } else {
    Serial.println("\n⏳ Time synchronized successfully!");
  }
}

void setup() {
  Serial.begin(19200);
  delay(2000);  // Allow Serial Monitor to start
  Serial.println("\n🚀 ESP32 is starting up...");

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    for(;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setRotation(2); // Flip screen 180 degrees

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

  // Initialize the DHT Sensor
  dht.begin();

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

  // Get the current time
  time_t now = time(nullptr);
  struct tm timeinfo;
  localtime_r(&now, &timeinfo);

  // Format the time as a string
  char timeStr[50];
  strftime(timeStr, sizeof(timeStr), "%m/%d/%y %I:%M%p PST", &timeinfo);

  // Read temperature and humidity
  float humidity = dht.readHumidity();
  float temperatureC = dht.readTemperature();
  float temperatureF = (temperatureC * 1.8) + 32;

  if (isnan(humidity) || isnan(temperatureC)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Serial output every 5 seconds
  if (millis() - lastSerialPrintTime >= serialInterval) {
    Serial.printf("%s | Humidity: %.2f %% | Temperature: %.2f °C | %.2f °F\n", 
                  timeStr, humidity, temperatureC, temperatureF);
    lastSerialPrintTime = millis();
  }

  // Send MQTT message every 5 minutes
  if (millis() - lastMQTTSentTime >= mqttInterval) {
    // Display MQTT sent message
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(10, 20);
    display.println("MQTT Message");
    display.setCursor(30, 40);
    display.println("Sent");
    display.display();
    delay(2000);  // Show message for 2 seconds
    char message[100];
    snprintf(message, sizeof(message), "%s | Humidity: %.2f %% | Temperature: %.2f °C | %.2f °F", 
             timeStr, humidity, temperatureC, temperatureF);
    client.publish(topic, message, true);
    lastMQTTSentTime = millis();
  }

   // Clear Display and Set Text Color to White
  display.clearDisplay();
  display.setTextColor(WHITE);  // Ensure all text is white

  // Display Sensor Data at the Top
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Humidity:");
  display.setCursor(10, 10);
  display.println(String(humidity, 1) + " %");
  display.setCursor(0, 20);
  display.println("Temperature:");
  display.setCursor(10, 30);
  display.println(String(temperatureC, 1) + " C");
  display.setCursor(10, 40);
  display.println(String(temperatureF, 1) + " F");


  // Display Date at the Bottom
  display.setCursor(0, 54);
  display.println(timeStr);

  // Refresh OLED Screen
  display.display();
}
