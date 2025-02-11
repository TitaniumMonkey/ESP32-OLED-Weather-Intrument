#include "include/mqtt_client.h"
#include "include/mqtt_publisher.h"
#include "include/dht_sensor.h"
#include "include/bmp390_sensor.h"
#include <Arduino.h>

// External declarations for MQTT client and timing
extern PubSubClient client;           // MQTT client instance
extern unsigned long mqttSentDisplayTime;  // Timestamp for last successful MQTT data send

// MQTT topic definitions for sensor state
const char* topic_state_temp = "homeassistant/sensor/bmp390_temperature/state";
const char* topic_state_humidity = "homeassistant/sensor/bmp390_humidity/state";
const char* topic_state_altitude = "homeassistant/sensor/bmp390_altitude/state";
const char* topic_state_pressure = "homeassistant/sensor/bmp390_pressure/state";

// MQTT topic definitions for sensor discovery
const char* topic_discovery_temp = "homeassistant/sensor/bmp390_temperature/config";
const char* topic_discovery_humidity = "homeassistant/sensor/bmp390_humidity/config";
const char* topic_discovery_altitude = "homeassistant/sensor/bmp390_altitude/config";
const char* topic_discovery_pressure = "homeassistant/sensor/bmp390_pressure/config";

/**
 * Publishes discovery messages for all sensors to Home Assistant
 * This function creates and sends MQTT discovery payloads for temperature,
 * humidity, pressure, and altitude sensors
 * 
 * @return bool Returns true if all discovery messages were sent successfully
 */
bool publishDiscoveryMessages() {
    if (!client.connected()) {
        Serial.println("⚠️ Cannot send discovery messages - MQTT not connected!");
        return false;
    }

    char discoveryPayload[512];
    bool success = true;

    // Temperature sensor discovery
    snprintf(discoveryPayload, sizeof(discoveryPayload),
        "{\"name\":\"BMP390 Temperature\","
        "\"state_topic\":\"homeassistant/sensor/bmp390_temperature/state\","
        "\"unique_id\":\"bmp390_temperature\","
        "\"unit_of_measurement\":\"°F\","
        "\"device_class\":\"temperature\","
        "\"value_template\":\"{{ value_json.temperature }}\"}");
    
    success &= client.publish("homeassistant/sensor/bmp390_temperature/config", discoveryPayload, true);
    delay(200);

    // Humidity sensor discovery
    snprintf(discoveryPayload, sizeof(discoveryPayload),
        "{\"name\":\"BMP390 Humidity\","
        "\"state_topic\":\"homeassistant/sensor/bmp390_humidity/state\","
        "\"unique_id\":\"bmp390_humidity\","
        "\"unit_of_measurement\":\"%\","
        "\"device_class\":\"humidity\","
        "\"value_template\":\"{{ value_json.humidity }}\"}");
    
    success &= client.publish("homeassistant/sensor/bmp390_humidity/config", discoveryPayload, true);
    delay(200);

    // Pressure sensor discovery
    snprintf(discoveryPayload, sizeof(discoveryPayload),
        "{\"name\":\"BMP390 Pressure\","
        "\"state_topic\":\"homeassistant/sensor/bmp390_pressure/state\","
        "\"unique_id\":\"bmp390_pressure\","
        "\"unit_of_measurement\":\"hPa\","
        "\"device_class\":\"pressure\","
        "\"value_template\":\"{{ value_json.pressure }}\"}");
    
    success &= client.publish("homeassistant/sensor/bmp390_pressure/config", discoveryPayload, true);
    delay(200);

    // Altitude sensor discovery
    snprintf(discoveryPayload, sizeof(discoveryPayload),
        "{\"name\":\"BMP390 Altitude\","
        "\"state_topic\":\"homeassistant/sensor/bmp390_altitude/state\","
        "\"unique_id\":\"bmp390_altitude\","
        "\"unit_of_measurement\":\"ft\","
        "\"icon\":\"mdi:altimeter\","
        "\"value_template\":\"{{ value_json.altitude }}\"}");
    
    success &= client.publish("homeassistant/sensor/bmp390_altitude/config", discoveryPayload, true);

    if (success) {
        Serial.println("✅ MQTT Discovery messages sent successfully!");
    } else {
        Serial.println("⚠️ Failed to send discovery messages!");
    }
    
    return success;
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
    }
}
