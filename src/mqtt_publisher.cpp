#include "include/mqtt_client.h"
#include "include/mqtt_publisher.h"
#include "include/dht_sensor.h"
#include "include/bmp180_sensor.h"
#include <Arduino.h>

extern PubSubClient client;
extern unsigned long mqttSentDisplayTime;

const char* topic_state_temp = "homeassistant/sensor/esp32_temperature/state";
const char* topic_state_humidity = "homeassistant/sensor/esp32_humidity/state";
const char* topic_state_altitude = "homeassistant/sensor/esp32_altitude/state";
const char* topic_state_pressure = "homeassistant/sensor/esp32_pressure/state";

const char* topic_discovery_temp = "homeassistant/sensor/esp32_temperature/config";
const char* topic_discovery_humidity = "homeassistant/sensor/esp32_humidity/config";
const char* topic_discovery_altitude = "homeassistant/sensor/esp32_altitude/config";
const char* topic_discovery_pressure = "homeassistant/sensor/esp32_pressure/config";

// ✅ Last working version of publishDiscoveryMessages()
void publishDiscoveryMessages() {
    char discoveryPayload[256];

    snprintf(discoveryPayload, sizeof(discoveryPayload),
        "{"
        "\"name\": \"ESP32 Temperature\","
        "\"unique_id\": \"esp32_temperature\","
        "\"state_topic\": \"%s\","
        "\"unit_of_measurement\": \"°F\","
        "\"device_class\": \"temperature\","
        "\"value_template\": \"{{ value_json.temperature }}\""
        "}", topic_state_temp);
    client.publish(topic_discovery_temp, discoveryPayload, true);

    snprintf(discoveryPayload, sizeof(discoveryPayload),
        "{"
        "\"name\": \"ESP32 Humidity\","
        "\"unique_id\": \"esp32_humidity\","
        "\"state_topic\": \"%s\","
        "\"unit_of_measurement\": \"%%\","
        "\"device_class\": \"humidity\","
        "\"value_template\": \"{{ value_json.humidity }}\""
        "}", topic_state_humidity);
    client.publish(topic_discovery_humidity, discoveryPayload, true);

    snprintf(discoveryPayload, sizeof(discoveryPayload),
        "{"
        "\"name\": \"ESP32 Pressure\","
        "\"unique_id\": \"esp32_pressure\","
        "\"state_topic\": \"%s\","
        "\"unit_of_measurement\": \"hPa\","
        "\"device_class\": \"pressure\","
        "\"value_template\": \"{{ value_json.pressure }}\""
        "}", topic_state_pressure);
    client.publish(topic_discovery_pressure, discoveryPayload, true);

    snprintf(discoveryPayload, sizeof(discoveryPayload),
        "{"
        "\"name\": \"ESP32 Altitude\","
        "\"unique_id\": \"esp32_altitude\","
        "\"state_topic\": \"%s\","
        "\"unit_of_measurement\": \"ft\","
        "\"icon\": \"mdi:altimeter\","
        "\"value_template\": \"{{ value_json.altitude }}\""
        "}", topic_state_altitude);
    client.publish(topic_discovery_altitude, discoveryPayload, true);

    Serial.println("✅ MQTT Discovery Sent for 4 Sensors!");
}

// ✅ Last working version of publishSensorData()
void publishSensorData() {
    if (!client.connected()) {
        Serial.println("⚠️ MQTT Publish Failed! Retrying in 5 seconds...");
        vTaskDelay(pdMS_TO_TICKS(5000)); // ✅ Retry in 5 seconds if disconnected
        return;
    }

    float temperatureF = (temperature * 1.8) + 32;
    float altitudeFt = altitude * 3.28084;

    char payload[50];
    bool success = true;

    snprintf(payload, sizeof(payload), "{ \"temperature\": %.2f }", temperatureF);
    if (!client.publish(topic_state_temp, payload, true)) success = false;

    snprintf(payload, sizeof(payload), "{ \"humidity\": %.1f }", humidity);
    if (!client.publish(topic_state_humidity, payload, true)) success = false;

    snprintf(payload, sizeof(payload), "{ \"altitude\": %.0f }", altitudeFt);
    if (!client.publish(topic_state_altitude, payload, true)) success = false;

    snprintf(payload, sizeof(payload), "{ \"pressure\": %.0f }", pressure);
    if (!client.publish(topic_state_pressure, payload, true)) success = false;

    if (success) {
        mqttSentDisplayTime = millis();
        Serial.println("📡 MQTT Sensor Data Published!");
    }
}