#include <WiFi.h>
#include "include/mqtt_client.h"
#include "include/mqtt_publisher.h"
#include <Arduino.h>
#include "include/secrets.h"
#include <PubSubClient.h>

// Global WiFi and MQTT client instances
WiFiClient espClient;
PubSubClient client(espClient);

/**
 * Sets up and establishes MQTT connection
 * This function configures the MQTT server, attempts to connect,
 * and handles discovery message publication on first connection
 */
void setupMQTT() {
    static bool firstBoot = true;

    client.setBufferSize(512);  // Increase buffer size for larger messages
    client.setServer(MQTT_SERVER, 1883);
    
    if (client.connect("ESP32WeatherStation", MQTT_USER, MQTT_PASS)) {
        if (firstBoot) {
            Serial.println("Connected to MQTT broker!");
            Serial.printf("MQTT Buffer Size: %d\n", client.getBufferSize());
            firstBoot = false;
        }
    } else {
        Serial.printf("⚠️ MQTT Connection Failed! Error: %d\n", client.state());
    }
}
