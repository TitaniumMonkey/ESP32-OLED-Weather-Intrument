#ifndef DHT_SENSOR_H
#define DHT_SENSOR_H

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 4          // GPIO pin connected to DHT11
#define DHTTYPE DHT11     // DHT sensor type

extern float humidity;

void setupDHTSensor();
void readDHTSensor();

#endif // DHT_SENSOR_H