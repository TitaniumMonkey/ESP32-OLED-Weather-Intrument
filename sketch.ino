#include <Arduino.h>
#include "include/wifi_manager.h"
#include "include/dht_sensor.h"
#include "include/bmp390_sensor.h"
#include "include/mqtt_client.h"
#include "include/mqtt_publisher.h"
#include "include/oled_display.h"
#include "include/time_manager.h"

// Task handle declarations for FreeRTOS tasks
TaskHandle_t wifiTaskHandle;
TaskHandle_t dhtTaskHandle;
TaskHandle_t bmpTaskHandle;
TaskHandle_t mqttTaskHandle;
TaskHandle_t oledTaskHandle;
TaskHandle_t serialOutputTaskHandle;

// Timing variable for serial output
unsigned long lastSerialPrintTime = 0;

/**
 * Wi-Fi task: Maintains Wi-Fi connection
 */
void wifiTask(void *pvParameters) {
    setupWiFi();
    while (1) {
        maintainWiFi(); // Keep Wi-Fi connected
        vTaskDelay(pdMS_TO_TICKS(10000)); // Check Wi-Fi every 10 seconds
    }
}

/**
 * DHT Sensor Task: Reads humidity sensor
 */
void dhtTask(void *pvParameters) {
    vTaskDelay(pdMS_TO_TICKS(2000));  // Initial delay for sensor initialization
    while (1) {
        readDHTSensor();
        vTaskDelay(pdMS_TO_TICKS(2000)); // Read every 2 seconds
    }
}

/**
 * BMP390 Sensor Task: Reads temperature, pressure and altitude
 */
void bmpTask(void *pvParameters) {
    while (1) {
        readBMP390Sensor();
        vTaskDelay(pdMS_TO_TICKS(5000)); // Read every 5 seconds
    }
}

/**
 * MQTT Task: Handles MQTT communication
 */
void mqttTask(void *pvParameters) {
    setupMQTT();  // Initial setup here
    int reconnectAttempts = 0;
    
    // Delay for 1 minute to allow sensors to stabilize before first publish
    vTaskDelay(pdMS_TO_TICKS(60000)); 
    
    while (1) {
        if (!client.connected()) {
            // Attempt to reconnect up to 3 times
            while (!client.connected() && reconnectAttempts < 3) {
                setupMQTT();  // Attempt to reconnect immediately
                reconnectAttempts++;
                if (!client.connected()) {
                    // No output here, just increment the attempt counter
                    vTaskDelay(pdMS_TO_TICKS(1666)); // Wait 1.666 seconds (5/3 seconds) between attempts
                }
            }

            // If after 3 attempts it's still not connected, print an error
            if (!client.connected()) {
                Serial.println("⚠️ MQTT Reconnection Failed after 3 attempts! Retrying in 5 seconds...");
                reconnectAttempts = 0;  // Reset the counter
                vTaskDelay(pdMS_TO_TICKS(5000));
            } else {
                reconnectAttempts = 0;  // Reset the counter if connected
            }
        }

        if (client.connected()) {
            publishSensorData();
            // Add the success message here
            Serial.println("📡 MQTT Sensor Data Published!");
        } else {
            // This else block should not be reached if the above logic is correct
            // But included for completeness
            vTaskDelay(pdMS_TO_TICKS(5000));
            continue;
        }

        client.loop();
        vTaskDelay(pdMS_TO_TICKS(300000)); // Publish every 5 minutes
    }
}

/**
 * Serial output task: Prints sensor data periodically
 */
void serialOutputTask(void *pvParameters) {
    while (1) {
        unsigned long currentTime = millis();

        if (currentTime - lastSerialPrintTime >= 60000) {  // Print every 1 minute
            lastSerialPrintTime = currentTime;

            updateTimeString();  // Ensure timestamp is updated

            // Convert and Smooth Values
            float temperatureF = (temperature * 9 / 5) + 32;
            float altitudeFt = altitude * 3.28084;
            float smoothedAltitudeM = altitude;  // Placeholder for smoothing logic if needed
            float smoothedAltitudeF = altitudeFt;

            // Print Serial Output in Correct Order
            Serial.printf("%s | Temp: %.2f C / %.2f F | Humidity: %.1f%% | Alt: %.0f m / %.0f ft | Pressure: %.0f hPa\n",
                          getTimeString(), temperature, temperatureF, humidity, smoothedAltitudeM, smoothedAltitudeF, pressure);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));  // Task runs every second, but serial prints every minute
    }
}

// Declaration of oledTask function before setup
void oledTask(void *pvParameters);

void setup() {
    Serial.begin(19200);
    
    setupWiFi();
    setupTime();
    setupDHTSensor();
    setupBMP390Sensor();
    setupMQTT();
    setupOLED();

    // Initialize FreeRTOS Tasks
    xTaskCreatePinnedToCore(wifiTask, "WiFiTask", 4096, NULL, 1, &wifiTaskHandle, 0);
    xTaskCreatePinnedToCore(dhtTask, "DHTTask", 3072, NULL, 1, &dhtTaskHandle, 1);
    xTaskCreatePinnedToCore(bmpTask, "BMPTask", 2048, NULL, 1, &bmpTaskHandle, 1);
    xTaskCreatePinnedToCore(mqttTask, "MQTTTask", 4096, NULL, 1, &mqttTaskHandle, 0);
    xTaskCreatePinnedToCore(oledTask, "OLEDTask", 4096, NULL, 1, &oledTaskHandle, 1);
    xTaskCreatePinnedToCore(serialOutputTask, "SerialOutputTask", 4096, NULL, 1, &serialOutputTaskHandle, 0);
}

void loop() {
    vTaskDelay(pdMS_TO_TICKS(1000));
}

// Implementation of oledTask after setup
void oledTask(void *pvParameters) {
    while (1) {
        updateOLED();
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}