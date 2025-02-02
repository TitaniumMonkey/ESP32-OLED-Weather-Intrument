#include "include/dht_sensor.h"

// Global DHT sensor instance and humidity variable
DHT dht(DHTPIN, DHTTYPE);  // DHT sensor object with predefined pin and type
float humidity = 0.0;      // Variable to store current humidity reading

/**
 * Initializes the DHT humidity sensor
 * This function starts the sensor and logs its initialization
 */
void setupDHTSensor() {
    dht.begin();  // Initialize DHT sensor communication
    Serial.println("DHT Sensor Initialized.");
}

/**
 * Reads data from the DHT humidity sensor
 * This function attempts to read humidity and stores valid readings
 */
void readDHTSensor() {
    float hum = dht.readHumidity();  // Attempt to read humidity value

    // Check if reading is valid
    if (!isnan(hum)) {
        humidity = hum;  // Store valid humidity reading
        // Note: Value is stored but not printed to avoid excessive logging
    } else {
        // Handle reading failure
        Serial.println("Failed to read from DHT sensor!");
        // Error message is printed only when reading fails
    }
}
