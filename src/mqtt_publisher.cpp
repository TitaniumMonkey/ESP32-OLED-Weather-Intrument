#include "include/mqtt_client.h"
#include "include/mqtt_publisher.h"
#include "include/dht_sensor.h"
#include "include/bmp180_sensor.h"
#include <Arduino.h>

// External declarations for MQTT client and timing
extern PubSubClient client;           // MQTT client instance
extern unsigned long mqttSentDisplayTime;  // Timestamp for last successful MQTT data send

// MQTT topic definitions for sensor state
const char* topic_state_temp = "homeassistant/sensor/esp32_temperature/state";
const char* topic_state_humidity = "homeassistant/sensor/esp32_humidity/state";
const char* topic_state_altitude = "homeassistant/sensor/esp32_altitude/state";
const char* topic_state_pressure = "homeassistant/sensor/esp32_pressure/state";

// MQTT topic definitions for sensor discovery
const char* topic_discovery_temp = "homeassistant/sensor/esp32_temperature/config";
const char* topic_discovery_humidity = "homeassistant/sensor/esp32_humidity/config";
const char* topic_discovery_altitude = "homeassistant/sensor/esp32_altitude/config";
const char* topic_discovery_pressure = "homeassistant/sensor/esp32_pressure/config";

/**
 * Publishes discovery messages for all sensors to Home Assistant
 * This function creates and sends MQTT discovery payloads for temperature,
 * humidity, pressure, and altitude sensors
 */
void publishDiscoveryMessages() {
    char discoveryPayload[256];  // Buffer for discovery payload

    // Temperature sensor discovery
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

    // Humidity sensor discovery
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

    // Pressure sensor discovery
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

    // Altitude sensor discovery
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

/**
 * Publishes current sensor data to MQTT topics
 * This function reads sensor values, converts units where necessary,
 * and publishes the data to Home Assistant
 */
void publishSensorData() {
    // Check MQTT connection status
    if (!client.connected()) {
        Serial.println("⚠️ MQTT Publish Failed! Retrying in 5 seconds...");
        vTaskDelay(pdMS_TO_TICKS(5000));  // Wait 5 seconds before retry
        return;
    }

    // Convert temperature to Fahrenheit and altitude to feet
    float temperatureF = (temperature * 1.8) + 32;
    float altitudeFt = altitude * 3.28084;

    char payload[50];  // Buffer for sensor data payload
    bool success = true;  // Flag to track publish success

    // Publish temperature data
    snprintf(payload, sizeof(payload), "{ \"temperature\": %.2f }", temperatureF);
    if (!client.publish(topic_state_temp, payload, true)) success = false;

    // Publish humidity data
    snprintf(payload, sizeof(payload), "{ \"humidity\": %.1f }", humidity);
    if (!client.publish(topic_state_humidity, payload, true)) success = false;

    // Publish altitude data
    snprintf(payload, sizeof(payload), "{ \"altitude\": %.0f }", altitudeFt);
    if (!client.publish(topic_state_altitude, payload, true)) success = false;

    // Publish pressure data
    snprintf(payload, sizeof(payload), "{ \"pressure\": %.0f }", pressure);
    if (!client.publish(topic_state_pressure, payload, true)) success = false;

    // Log success status
    if (success) {
        mqttSentDisplayTime = millis();  // Update last successful send time
        Serial.println("📡 MQTT Sensor Data Published!");
    }
}
