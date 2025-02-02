#include <WiFi.h>
#include "include/mqtt_client.h"
#include "include/mqtt_publisher.h"
#include <Arduino.h>

// Global WiFi and MQTT client instances
WiFiClient espClient;        // WiFi client for network communication
PubSubClient client(espClient);  // MQTT client using the WiFi connection

/**
 * Sets up and establishes MQTT connection
 * This function configures the MQTT server, attempts to connect,
 * and handles discovery message publication on first connection
 */
void setupMQTT() {
    // Configure MQTT server address and port
    client.setServer(MQTT_SERVER, 1883);

    // Attempt to connect to MQTT broker
    Serial.print("Connecting to MQTT broker...");
    
    // Continue trying to connect until successful
    while (!client.connected()) {
        // Attempt connection with device ID and credentials
        if (client.connect("ESP32WeatherStation", MQTT_USER, MQTT_PASS)) {
            Serial.println(" Connected!");

            // Flag to ensure discovery messages are sent only once on boot
            static bool discoverySent = false;
            
            // Publish discovery messages only on first successful connection
            if (!discoverySent) {
                publishDiscoveryMessages();  // Announce sensor configurations to Home Assistant
                discoverySent = true;        // Prevent repeated discovery messages
            }
        } else {
            // Handle connection failure
            Serial.println("⚠️ MQTT Connection Failed! Retrying in 5 seconds...");
            delay(5000);  // Wait 5 seconds before retrying
        }
    }
}
