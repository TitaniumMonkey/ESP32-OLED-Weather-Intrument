#include <WiFi.h>
#include "include/mqtt_client.h"
#include "include/mqtt_publisher.h"
#include <Arduino.h>

WiFiClient espClient;
PubSubClient client(espClient);

void setupMQTT() {
    client.setServer(MQTT_SERVER, 1883);

    Serial.print("Connecting to MQTT broker...");
    while (!client.connected()) {
        if (client.connect("ESP32WeatherStation", MQTT_USER, MQTT_PASS)) {
            Serial.println(" Connected!");

            // ✅ Only send discovery messages on boot
            static bool discoverySent = false;
            if (!discoverySent) {
                publishDiscoveryMessages();
                discoverySent = true;
            }
        } else {
            Serial.println("⚠️ MQTT Connection Failed! Retrying in 5 seconds...");
            delay(5000);
        }
    }
}