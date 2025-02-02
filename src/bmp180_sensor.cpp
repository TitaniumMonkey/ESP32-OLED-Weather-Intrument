#include "include/bmp180_sensor.h"

// Global BMP180 sensor instance and measurement variables
Adafruit_BMP085 bmp;  // BMP180 pressure and temperature sensor object
float temperature = 0.0;  // Current temperature reading (°C)
float pressure = 0.0;     // Current pressure reading (hPa)
float altitude = 0.0;     // Current altitude reading (meters)

// Calibration offsets
float tempOffset = 0.0;     // Temperature offset for sensor drift compensation
float pressureOffset = 0.0; // Pressure offset for barometric calibration
float altitudeOffset = 0.0; // Altitude offset for known height difference

// Variables for smoothing calculations
float prevPressure = 0.0;   // Previous pressure reading for smoothing
float prevAltitude = 0.0;   // Previous altitude reading for smoothing

/**
 * Initializes the BMP180 pressure and temperature sensor
 * This function attempts to start the sensor and logs its status
 */
void setupBMP180Sensor() {
    if (!bmp.begin()) {
        Serial.println("BMP180 Sensor not detected!");
        // Indicates hardware or connection issue
    } else {
        Serial.println("BMP180 Sensor Initialized.");
        // Confirms successful sensor initialization
    }
}

/**
 * Applies exponential smoothing to sensor readings
 * @param newValue Current sensor reading
 * @param prevValue Previous smoothed value
 * @param alpha Smoothing factor (0-1, higher values give more weight to new readings)
 * @return Smoothed value
 */
float applySmoothing(float newValue, float prevValue, float alpha) {
    return (alpha * newValue) + ((1 - alpha) * prevValue);
}

/**
 * Reads and processes data from the BMP180 sensor
 * This function reads raw sensor values, applies offsets and smoothing,
 * and stores the results
 */
void readBMP180Sensor() {
    if (bmp.begin()) {
        // Read raw sensor values
        float rawTemp = bmp.readTemperature();          // °C
        float rawPressure = bmp.readPressure() / 100.0; // Convert Pa to hPa
        float rawAltitude = bmp.readAltitude();         // Meters

        // Apply calibration offsets
        temperature = rawTemp + tempOffset;
        pressure = rawPressure + pressureOffset;
        altitude = rawAltitude + altitudeOffset;

        // Apply exponential smoothing to reduce noise
        pressure = applySmoothing(pressure, prevPressure, 0.2);
        altitude = applySmoothing(altitude, prevAltitude, 0.2);
        // Smoothing factor (0.2) can be adjusted based on desired responsiveness

        // Store current values for next smoothing iteration
        prevPressure = pressure;
        prevAltitude = altitude;
    } else {
        Serial.println("Failed to read from BMP180 sensor!");
        // Indicates failure to communicate with sensor
    }
}
