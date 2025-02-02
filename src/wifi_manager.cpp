#include "include/wifi_manager.h"

/**
 * Sets up Wi-Fi connection
 */
void setupWiFi() {
    Serial.println("Connecting to Wi-Fi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    int retryCount = 0;
    // Attempt connection with timeout (20 retries, 500ms each)
    while (WiFi.status() != WL_CONNECTED && retryCount < 20) {
        delay(500);
        Serial.print(".");
        retryCount++;
    }

    // Check connection status
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWi-Fi Connected!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\nWi-Fi Connection Failed!");
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
