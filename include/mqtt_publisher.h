#ifndef MQTT_PUBLISHER_H
#define MQTT_PUBLISHER_H

#include <PubSubClient.h>

// External MQTT client declaration
extern PubSubClient client;

// Function declarations
void publishMQTTStatus(bool online);    // Publishes device online/offline status
void publishDiscoveryMessages();        // Publishes sensor discovery messages
void publishSensorData();               // Publishes current sensor readings

#endif // MQTT_PUBLISHER_H
