#include "include/time_manager.h"
#include <WiFi.h>
#include <Arduino.h>

// NTP Configuration
const char* ntpServer = "pool.ntp.org";  // ✅ Use an NTP server for accurate time
const long gmtOffset_sec = -8 * 3600;    // ✅ Adjust for Pacific Standard Time (PST)
const int daylightOffset_sec = 3600;     // ✅ Adjust for daylight savings (1 hour)

// **Time Storage**
static char timeStr[30] = "";  // ✅ Increased size to include AM/PM and PST

void setupTime() {
    Serial.println("Waiting 5 seconds before attempting NTP sync...");
    delay(5000);  // ✅ Wait before first connection attempt

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    struct tm timeInfo;
    while (!getLocalTime(&timeInfo)) {  // ✅ Retry until time is obtained
        Serial.println("Failed to obtain time from NTP. Retrying in 5 seconds...");
        delay(5000);
    }

    Serial.println("NTP Time Sync Setup Complete.");
}

void updateTimeString() {
    struct tm timeInfo;
    
    if (!getLocalTime(&timeInfo)) {
        Serial.println("Failed to obtain time from NTP");
        return;
    }

    // ✅ Format time correctly with AM/PM and PST
    strftime(timeStr, sizeof(timeStr), "%m/%d/%y %I:%M %p PST", &timeInfo);
}

const char* getTimeString() {
    return timeStr;
}