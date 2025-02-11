#ifndef BMP390_SENSOR_H
#define BMP390_SENSOR_H

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP3XX.h>

// External variable declarations for sensor readings
extern float temperature;    // Current temperature (°C)
extern float pressure;       // Current pressure (hPa)
extern float altitude;       // Current altitude (meters)

// Function declarations
void setupBMP390Sensor();    // Initializes BMP390 sensor
void readBMP390Sensor();     // Reads and processes sensor data
float applySmoothing(float newValue, float prevValue, float alpha); // Applies exponential smoothing

// External calibration offset declarations
extern float tempOffset;     // Temperature calibration offset
extern float pressureOffset; // Pressure calibration offset
extern float altitudeOffset; // Altitude calibration offset

#endif // BMP390_SENSOR_H