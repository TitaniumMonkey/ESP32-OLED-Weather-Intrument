#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include "include/secrets.h"  // Includes Wi-Fi credentials

// Function declarations
void setupWiFi();     // Sets up Wi-Fi connection
void maintainWiFi();  // Maintains Wi-Fi connection

#endif // WIFI_MANAGER_H
