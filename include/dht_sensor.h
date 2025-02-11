#ifndef DHT_SENSOR_H
#define DHT_SENSOR_H

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// DHT sensor configuration
#define DHTPIN 4          // GPIO pin for DHT sensor connection
#define DHTTYPE DHT11     // Specifies DHT11 sensor type

// External variable declaration
extern float humidity;    // Current humidity reading (%)

// Function declarations
void setupDHTSensor();    // Initializes DHT sensor
void readDHTSensor();     // Reads humidity from DHT sensor

#endif // DHT_SENSOR_H
