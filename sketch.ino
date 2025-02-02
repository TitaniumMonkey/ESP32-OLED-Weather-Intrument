#include <Arduino.h>
#include "include/wifi_manager.h"
#include "include/dht_sensor.h"
#include "include/bmp180_sensor.h"
#include "include/mqtt_client.h"
#include "include/mqtt_publisher.h"
#include "include/oled_display.h"
#include "include/time_manager.h"

// Task Handles
TaskHandle_t wifiTaskHandle;
TaskHandle_t dhtTaskHandle;
TaskHandle_t bmpTaskHandle;
TaskHandle_t mqttTaskHandle;
TaskHandle_t oledTaskHandle;
TaskHandle_t serialOutputTaskHandle;

// **Last Serial Print Timestamp (For 15s interval)**
unsigned long lastSerialPrintTime = 0;

// **Wi-Fi Task**
void wifiTask(void *pvParameters) {
    setupWiFi();
    while (1) {
        maintainWiFi(); // Keep Wi-Fi connected
        vTaskDelay(pdMS_TO_TICKS(10000)); // Check Wi-Fi every 10 seconds
    }
}

// **DHT Sensor Task**
void dhtTask(void *pvParameters) {
    vTaskDelay(pdMS_TO_TICKS(2000));  // ✅ Allow time for sensor to initialize
    while (1) {
        readDHTSensor();
        vTaskDelay(pdMS_TO_TICKS(2000)); // Read every 2 seconds
    }
}

// **BMP180 Sensor Task**
void bmpTask(void *pvParameters) {
    while (1) {
        readBMP180Sensor();
        vTaskDelay(pdMS_TO_TICKS(5000)); // Read every 5 seconds
    }
}

// **MQTT Task**
void mqttTask(void *pvParameters) {
    setupMQTT();  // ✅ Remove `true`, just call `setupMQTT()`
    publishDiscoveryMessages();  // ✅ Send discovery messages only on boot

    while (1) {
        if (!client.connected()) {
            setupMQTT();  // ✅ Remove `false`, just call `setupMQTT()`
        }

        if (client.connected()) {  
            publishSensorData();
        } else {
            Serial.println("⚠️ MQTT Publish Failed! Retrying in 5 seconds...");
            vTaskDelay(pdMS_TO_TICKS(5000));  // ✅ Retry in 5 seconds if disconnected
            continue;  // ✅ Skip this cycle and retry later
        }

        client.loop();  // ✅ Keep connection alive
        vTaskDelay(pdMS_TO_TICKS(60000)); // ✅ Publish every 1 minute
    }
}

// **OLED Display Task**
void oledTask(void *pvParameters) {
    while (1) {
        updateOLED();
        vTaskDelay(pdMS_TO_TICKS(3000)); // Update every 3 seconds
    }
}

void serialOutputTask(void *pvParameters) {
    while (1) {
        unsigned long currentTime = millis();

        if (currentTime - lastSerialPrintTime >= 15000) {  // ✅ Print every 15 sec
            lastSerialPrintTime = currentTime;

            updateTimeString();  // ✅ Ensure timestamp is updated

            // **Convert and Smooth Values**
            float temperatureF = (temperature * 9 / 5) + 32;
            float altitudeFt = altitude * 3.28084;
            float smoothedAltitudeM = altitude;  // Placeholder for smoothing logic if needed
            float smoothedAltitudeF = altitudeFt;

            // **Print Serial Output in Correct Order**
            Serial.printf("%s | Temp: %.2f C / %.2f F | Humidity: %.1f%% | Alt: %.0f m / %.0f ft | Pressure: %.0f hPa\n",
                          getTimeString(), temperature, temperatureF, humidity, smoothedAltitudeM, smoothedAltitudeF, pressure);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));  // ✅ Task runs every second, but prints every 15s
    }
}

void setup() {
    Serial.begin(19200);
    
    setupWiFi();  // ✅ Ensure Wi-Fi starts before tasks
    setupTime();  // ✅ Sync time with NTP
    setupDHTSensor();  // ✅ Ensure DHT is initialized before running the task
    setupBMP180Sensor();  // ✅ Ensure BMP180 is initialized too
    setupMQTT();  // ✅ Remove `true`, just call `setupMQTT()`
    setupOLED();  // ✅ Ensures OLED is ready before task runs

    // **Initialize FreeRTOS Tasks**
    xTaskCreatePinnedToCore(wifiTask, "WiFiTask", 4096, NULL, 1, &wifiTaskHandle, 0);
    xTaskCreatePinnedToCore(dhtTask, "DHTTask", 3072, NULL, 1, &dhtTaskHandle, 1);
    xTaskCreatePinnedToCore(bmpTask, "BMPTTask", 2048, NULL, 1, &bmpTaskHandle, 1);
    xTaskCreatePinnedToCore(mqttTask, "MQTTTask", 4096, NULL, 1, &mqttTaskHandle, 0);
    xTaskCreatePinnedToCore(oledTask, "OLEDTask", 4096, NULL, 1, &oledTaskHandle, 1);
    xTaskCreatePinnedToCore(serialOutputTask, "SerialOutputTask", 4096, NULL, 1, &serialOutputTaskHandle, 0);
}

void loop() {
    vTaskDelay(pdMS_TO_TICKS(1000)); // Reduce CPU usage
}
