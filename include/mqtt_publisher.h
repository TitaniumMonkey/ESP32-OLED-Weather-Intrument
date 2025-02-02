#ifndef MQTT_PUBLISHER_H
#define MQTT_PUBLISHER_H

#include <PubSubClient.h>

extern PubSubClient client;

// ✅ Keep the last working function declarations
void publishMQTTStatus(bool online);
void publishDiscoveryMessages();
void publishSensorData();

#endif // MQTT_PUBLISHER_H