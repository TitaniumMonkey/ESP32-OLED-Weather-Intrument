#ifndef BMP180_SENSOR_H
#define BMP180_SENSOR_H

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085.h>

// External variable declarations for sensor readings
extern float temperature;    // Current temperature (°C)
extern float pressure;       // Current pressure (hPa)
extern float altitude;       // Current altitude (meters)

// Function declarations
void setupBMP180Sensor();    // Initializes BMP180 sensor
void readBMP180Sensor();     // Reads and processes sensor data
float applySmoothing(float newValue, float prevValue, float alpha); // Applies exponential smoothing

// External calibration offset declarations
extern float tempOffset;     // Temperature calibration offset
extern float pressureOffset; // Pressure calibration offset
extern float altitudeOffset; // Altitude calibration offset

#endif // BMP180_SENSOR_H
