#include "include/wifi_manager.h"

/**
 * Sets up Wi-Fi connection
 */
void setupWiFi() {
    Serial.println("Connecting to Wi-Fi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    int retryCount = 0;
    while (WiFi.status() != WL_CONNECTED && retryCount < 30) {  // Increased timeout
        delay(500);
        Serial.print(".");
        retryCount++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWi-Fi Connected!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
        delay(1000);  // Give the connection a moment to stabilize
    } else {
        Serial.println("\nWi-Fi Connection Failed!");
        ESP.restart();  // Restart if we can't connect to WiFi
    }
}

/**
 * Maintains Wi-Fi connection
 */
void maintainWiFi() {
    // Check if Wi-Fi connection is lost
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Wi-Fi Lost! Reconnecting...");
        WiFi.disconnect();
        WiFi.reconnect();
    }
}
