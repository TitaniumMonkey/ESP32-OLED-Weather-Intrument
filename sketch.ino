#include <Arduino.h>
#include "include/wifi_manager.h"
#include "include/dht_sensor.h"
#include "include/bmp180_sensor.h"
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

// **Wi-Fi task: Maintains Wi-Fi connection
void wifiTask(void *pvParameters) {
    setupWiFi();
    while (1) {
        maintainWiFi(); // Keep Wi-Fi connected
        vTaskDelay(pdMS_TO_TICKS(10000)); // Check Wi-Fi every 10 seconds
    }
}

// **DHT Sensor Task: Reads humidity sensor**
void dhtTask(void *pvParameters) {
    vTaskDelay(pdMS_TO_TICKS(2000));  //  Initial delay for sensor initialization
    while (1) {
        readDHTSensor();
        vTaskDelay(pdMS_TO_TICKS(2000)); // Read every 2 seconds
    }
}

// **BMP180 Sensor Task: Reads pressure, temperature, and altitude**
void bmpTask(void *pvParameters) {
    while (1) {
        readBMP180Sensor();
        vTaskDelay(pdMS_TO_TICKS(5000)); // Read every 5 seconds
    }
}

// **MQTT Task: Handles MQTT communication**
void mqttTask(void *pvParameters) {
    setupMQTT();  //  Remove `true`, just call `setupMQTT()`
    publishDiscoveryMessages();  //  Send discovery messages only on boot

    while (1) {
        if (!client.connected()) {
            setupMQTT();  //  Remove `false`, just call `setupMQTT()`
        }

        if (client.connected()) {  
            publishSensorData();
        } else {
            Serial.println("⚠️ MQTT Publish Failed! Retrying in 5 seconds...");
            vTaskDelay(pdMS_TO_TICKS(5000));  //  Retry in 5 seconds if disconnected
            continue;  //  Skip this cycle and retry later
        }

        client.loop();  //  Keep connection alive
        vTaskDelay(pdMS_TO_TICKS(300000)); //  Publish every 5 minutes
    }
}

// **OLED Display Task: Updates display content**
void oledTask(void *pvParameters) {
    while (1) {
        updateOLED();
        vTaskDelay(pdMS_TO_TICKS(3000)); // Update every 3 seconds
    }
}

// **Serial output task: Prints sensor data periodically**
void serialOutputTask(void *pvParameters) {
    while (1) {
        unsigned long currentTime = millis();

        if (currentTime - lastSerialPrintTime >= 60000) {  //  Print every 1 minute
            lastSerialPrintTime = currentTime;

            updateTimeString();  //  Ensure timestamp is updated

            // **Convert and Smooth Values**
            float temperatureF = (temperature * 9 / 5) + 32;
            float altitudeFt = altitude * 3.28084;
            float smoothedAltitudeM = altitude;  // Placeholder for smoothing logic if needed
            float smoothedAltitudeF = altitudeFt;

            // **Print Serial Output in Correct Order**
            Serial.printf("%s | Temp: %.2f C / %.2f F | Humidity: %.1f%% | Alt: %.0f m / %.0f ft | Pressure: %.0f hPa\n",
                          getTimeString(), temperature, temperatureF, humidity, smoothedAltitudeM, smoothedAltitudeF, pressure);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));  //  Task runs every second, but serial prints every minute
    }
}

// **Setup function: Initializes all components and tasks**
void setup() {
    Serial.begin(19200);
    
    // **Initialize hardware and services**
    setupWiFi();  //  Ensure Wi-Fi starts before tasks
    setupTime();  //  Sync time with NTP
    setupDHTSensor();  //  Ensure DHT is initialized before running the task
    setupBMP180Sensor();  //  Ensure BMP180 is initialized too
    setupMQTT();  //  Remove `true`, just call `setupMQTT()`
    setupOLED();  //  Ensures OLED is ready before task runs

    // **Initialize FreeRTOS Tasks**
    xTaskCreatePinnedToCore(wifiTask, "WiFiTask", 4096, NULL, 1, &wifiTaskHandle, 0);
    xTaskCreatePinnedToCore(dhtTask, "DHTTask", 3072, NULL, 1, &dhtTaskHandle, 1);
    xTaskCreatePinnedToCore(bmpTask, "BMPTTask", 2048, NULL, 1, &bmpTaskHandle, 1);
    xTaskCreatePinnedToCore(mqttTask, "MQTTTask", 4096, NULL, 1, &mqttTaskHandle, 0);
    xTaskCreatePinnedToCore(oledTask, "OLEDTask", 4096, NULL, 1, &oledTaskHandle, 1);
    xTaskCreatePinnedToCore(serialOutputTask, "SerialOutputTask", 4096, NULL, 1, &serialOutputTaskHandle, 0);
}
// **Main loop: Reduces CPU usage**
void loop() {
    vTaskDelay(pdMS_TO_TICKS(1000));
}
