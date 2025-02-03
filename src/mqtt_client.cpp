#include <WiFi.h>
#include "include/mqtt_client.h"
#include "include/mqtt_publisher.h"
#include <Arduino.h>

// Global WiFi and MQTT client instances
WiFiClient espClient;
PubSubClient client(espClient);

/**
 * Sets up and establishes MQTT connection
 * This function configures the MQTT server, attempts to connect,
 * and handles discovery message publication on first connection
 */
void setupMQTT() {
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }

    static bool firstConnection = true;
    static bool firstBoot = true;
    static bool discoverySent = false;

    client.setServer(MQTT_SERVER, 1883);
    
    while (!client.connected()) {
        if (client.connect("ESP32WeatherStation", MQTT_USER, MQTT_PASS)) {
            if (firstConnection) {
                firstConnection = false;
                if (firstBoot) {
                    Serial.println("Connected to MQTT broker!");
                    firstBoot = false;
                }

                if (!discoverySent) {
                    publishDiscoveryMessages();
                    discoverySent = true;
                }
            }
        } else {
            Serial.println("⚠️ MQTT Connection Failed! Retrying in 5 seconds...");
            delay(5000);
        }
    }
}