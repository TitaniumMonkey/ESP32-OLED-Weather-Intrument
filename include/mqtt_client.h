#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <WiFi.h>
#include <PubSubClient.h>
#include "secrets.h"

// External client declarations
extern WiFiClient espClient;     // WiFi client instance
extern PubSubClient client;      // MQTT client instance

// Function declaration
void setupMQTT();                // Sets up MQTT connection

#endif // MQTT_CLIENT_H
