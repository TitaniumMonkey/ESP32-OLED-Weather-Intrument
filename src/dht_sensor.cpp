#include "include/dht_sensor.h"

DHT dht(DHTPIN, DHTTYPE);
float humidity = 0.0;

void setupDHTSensor() {
    dht.begin();
    Serial.println("DHT Sensor Initialized.");
}

void readDHTSensor() {
    float hum = dht.readHumidity();

    if (!isnan(hum)) {
        humidity = hum;  // ✅ Store the humidity value, but do not print
    } else {
        Serial.println("Failed to read from DHT sensor!");  // ✅ Only print if it fails
    }
}