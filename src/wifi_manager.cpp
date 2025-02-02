#include "include/wifi_manager.h"

void setupWiFi() {
    Serial.println("Connecting to Wi-Fi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    int retryCount = 0;
    while (WiFi.status() != WL_CONNECTED && retryCount < 20) {
        delay(500);
        Serial.print(".");
        retryCount++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWi-Fi Connected!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\nWi-Fi Connection Failed!");
    }
}

void maintainWiFi() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Wi-Fi Lost! Reconnecting...");
        WiFi.disconnect();
        WiFi.reconnect();
    }
}