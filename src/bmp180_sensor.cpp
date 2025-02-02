#include "include/bmp180_sensor.h"

Adafruit_BMP085 bmp;
float temperature = 0.0;
float pressure = 0.0;
float altitude = 0.0;

// Default offsets (can be adjusted if needed)
float tempOffset = 0.0;     // Adjust for sensor drift
float pressureOffset = 0.0; // Adjust for barometric calibration
float altitudeOffset = 0.0; // Adjust for known altitude difference

// Previous values for smoothing
float prevPressure = 0.0;
float prevAltitude = 0.0;

void setupBMP180Sensor() {
    if (!bmp.begin()) {
        Serial.println("BMP180 Sensor not detected!");
    } else {
        Serial.println("BMP180 Sensor Initialized.");
    }
}

// Exponential Smoothing Function
float applySmoothing(float newValue, float prevValue, float alpha) {
    return (alpha * newValue) + ((1 - alpha) * prevValue);
}

void readBMP180Sensor() {
    if (bmp.begin()) {
        float rawTemp = bmp.readTemperature();
        float rawPressure = bmp.readPressure() / 100.0; // Convert to hPa
        float rawAltitude = bmp.readAltitude(); // Meters

        // Apply offsets
        temperature = rawTemp + tempOffset;
        pressure = rawPressure + pressureOffset;
        altitude = rawAltitude + altitudeOffset;

        // Apply smoothing to pressure and altitude
        pressure = applySmoothing(pressure, prevPressure, 0.2);  // Adjust smoothing factor if needed
        altitude = applySmoothing(altitude, prevAltitude, 0.2);

        // Store previous values for next iteration
        prevPressure = pressure;
        prevAltitude = altitude;
    } else {
        Serial.println("Failed to read from BMP180 sensor!");  // ✅ Keep error print only
    }
}