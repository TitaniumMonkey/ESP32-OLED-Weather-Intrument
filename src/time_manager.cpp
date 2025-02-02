#include "include/time_manager.h"
#include <WiFi.h>
#include <Arduino.h>

// NTP Configuration
const char* ntpServer = "pool.ntp.org";  // NTP server for time synchronization
const long gmtOffset_sec = -8 * 3600;    // Offset for Pacific Standard Time (PST)
const int daylightOffset_sec = 3600;     // Offset for daylight saving time (1 hour)

// Time string storage
static char timeStr[30] = "";  // Buffer for formatted time string (includes AM/PM and PST)

/**
 * Sets up NTP time synchronization
 */
void setupTime() {
    Serial.println("Waiting 5 seconds before attempting NTP sync...");
    delay(5000);  // Delay to ensure stable network connection

    // Configure time settings
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    struct tm timeInfo;
    // Retry until successful time synchronization
    while (!getLocalTime(&timeInfo)) {
        Serial.println("Failed to obtain time from NTP. Retrying in 5 seconds...");
        delay(5000);
    }

    Serial.println("NTP Time Sync Setup Complete.");
}

/**
 * Updates the formatted time string
 */
void updateTimeString() {
    struct tm timeInfo;
    
    if (!getLocalTime(&timeInfo)) {
        Serial.println("Failed to obtain time from NTP");
        return;
    }

    // Format time string with date, time, AM/PM indicator, and timezone
    strftime(timeStr, sizeof(timeStr), "%m/%d/%y %I:%M %p PST", &timeInfo);
}

/**
 * Returns the current formatted time string
 * @return Pointer to the time string
 */
const char* getTimeString() {
    return timeStr;
}
