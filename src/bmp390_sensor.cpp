#include "include/bmp390_sensor.h"

// Global BMP390 sensor instance and measurement variables
Adafruit_BMP3XX bmp;  // BMP390 pressure and temperature sensor object
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
 * Initializes the BMP390 pressure and temperature sensor
 * This function attempts to start the sensor and configures settings.
 */
void setupBMP390Sensor() {
    if (!bmp.begin_I2C()) {  // Use I2C initialization for BMP390
        Serial.println("❌ BMP390 Sensor not detected! Check wiring.");
        return;
    }

    Serial.println("✅ BMP390 Sensor Initialized.");

    // Configure BMP390 settings
    bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
    bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
    bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
    bmp.setOutputDataRate(BMP3_ODR_50_HZ);
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
 * Reads and processes data from the BMP390 sensor
 * This function reads raw sensor values, applies offsets and smoothing,
 * and stores the results
 */
void readBMP390Sensor() {
    if (!bmp.performReading()) {
        Serial.println("❌ Failed to read from BMP390 sensor!");
        return;
    }

    // Read raw sensor values
    float rawTemp = bmp.temperature;  // °C
    float rawPressure = bmp.pressure / 100.0; // Convert Pa to hPa

    // Calculate altitude using standard formula
    float rawAltitude = 44330 * (1.0 - pow(rawPressure / 1013.25, 0.1903));

    // Apply calibration offsets
    temperature = rawTemp + tempOffset;
    pressure = rawPressure + pressureOffset;
    altitude = rawAltitude + altitudeOffset;

    // Apply exponential smoothing to reduce noise
    pressure = applySmoothing(pressure, prevPressure, 0.2);
    altitude = applySmoothing(altitude, prevAltitude, 0.2);

    // Store current values for next smoothing iteration
    prevPressure = pressure;
    prevAltitude = altitude;
}
