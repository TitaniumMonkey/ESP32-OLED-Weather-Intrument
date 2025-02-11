#include "include/oled_display.h"
#include "include/time_manager.h"
#include "include/dht_sensor.h"
#include "include/bmp390_sensor.h"

#define BOOT_BUTTON_PIN 0  // ESP32 Boot Button (GPIO 0)

// Global OLED display instance and control variables
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
bool oledOn = true;                 // OLED power state
unsigned long oledTimer = 0;        // Timer for OLED auto-shutoff
unsigned long mqttSentDisplayTime = 0;  // Timestamp for MQTT send notification
volatile bool oledToggleRequested = false;  // Interrupt flag for OLED toggle

/**
 * Interrupt Service Routine (ISR) for button press
 * Sets flag to toggle OLED state
 */
void IRAM_ATTR handleButtonPress() {
    oledToggleRequested = true;
}

/**
 * Initializes the OLED display and button interrupt
 */
void setupOLED() {
    // Initialize OLED display
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println("SSD1306 OLED initialization failed!");
        return;
    }
    
    // Configure display settings
    display.setRotation(2);  // Rotate display 180 degrees
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.display();
    Serial.println("OLED Display Initialized.");

    // Configure button interrupt
    pinMode(BOOT_BUTTON_PIN, INPUT_PULLUP);
    attachInterrupt(BOOT_BUTTON_PIN, handleButtonPress, FALLING);
}

/**
 * Turns on the OLED display and resets the timer
 */
void turnOnOLED() {
    oledOn = true;
    oledTimer = millis();
    Serial.println("OLED turned ON.");
}

/**
 * Turns off the OLED display
 */
void turnOffOLED() {
    oledOn = false;
    display.clearDisplay();
    display.display();
    Serial.println("OLED turned OFF.");
}

/**
 * Updates the OLED display content
 */
void updateOLED() {
    // Handle OLED toggle request from button press
    if (oledToggleRequested) {
        oledToggleRequested = false;
        oledOn = !oledOn;
        if (oledOn) {
            turnOnOLED();
        } else {
            turnOffOLED();
        }
    }

    if (oledOn) {
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);

        // Convert units for display
        float temperatureF = (temperature * 9 / 5) + 32;
        float altitudeFt = altitude * 3.28084;

        // Display temperature
        display.setCursor(0, 0);
        display.printf("Temp: %.1f C / %.1f F\n", temperature, temperatureF);

        // Display humidity
        display.setCursor(0, 10);
        display.printf("Humidity: %.1f%%\n", humidity);

        // Display altitude
        display.setCursor(0, 20);
        display.printf("Alt: %.0f m / %.0f ft\n", altitude, altitudeFt);

        // Display pressure
        display.setCursor(0, 30);
        display.printf("Pressure: %.0f hPa\n", pressure);

        // Display MQTT send notification (for 5 seconds)
        if (millis() - mqttSentDisplayTime <= 5000) {
            display.setCursor(0, 40);
            display.println("MQTT Sent!");
        }

        // Display current timestamp
        updateTimeString();
        display.setCursor(0, 54);
        display.println(getTimeString());

        // Auto-shutoff after 5 minutes
        if (millis() - oledTimer >= 5 * 60 * 1000) {
            turnOffOLED();
        }

        display.display();
    } else {
        display.clearDisplay();
        display.display();
    }
}
