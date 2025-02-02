#ifndef BMP180_SENSOR_H
#define BMP180_SENSOR_H

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085.h>

extern float temperature;
extern float pressure;
extern float altitude;

void setupBMP180Sensor();
void readBMP180Sensor();
float applySmoothing(float newValue, float prevValue, float alpha); // New smoothing function

// Offset Values
extern float tempOffset;
extern float pressureOffset;
extern float altitudeOffset;

#endif // BMP180_SENSOR_H