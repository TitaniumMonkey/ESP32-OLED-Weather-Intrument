#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <WiFi.h>
#include <PubSubClient.h>
#include "secrets.h"

extern WiFiClient espClient;
extern PubSubClient client;

// ✅ Last working version of setupMQTT()
void setupMQTT();

#endif // MQTT_CLIENT_H
