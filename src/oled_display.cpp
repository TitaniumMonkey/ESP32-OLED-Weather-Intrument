#include "include/oled_display.h"
#include "include/time_manager.h"
#include "include/dht_sensor.h"
#include "include/bmp180_sensor.h"

#define BOOT_BUTTON_PIN 0  // ✅ ESP32 Boot Button (GPIO 0)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

bool oledOn = true;
unsigned long oledTimer = 0;
unsigned long mqttSentDisplayTime = 0;
volatile bool oledToggleRequested = false;  // ✅ Interrupt flag

// **Interrupt Service Routine (ISR)**
void IRAM_ATTR handleButtonPress() {
    oledToggleRequested = true;  // ✅ Set flag when button is pressed
}

void setupOLED() {
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println("SSD1306 OLED initialization failed!");
        return;
    }
    display.setRotation(2); // ✅ Flip OLED
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.display();
    Serial.println("OLED Display Initialized.");

    pinMode(BOOT_BUTTON_PIN, INPUT_PULLUP);
    attachInterrupt(BOOT_BUTTON_PIN, handleButtonPress, FALLING);  // ✅ Enable hardware interrupt
}

// **Function to Turn OLED ON**
void turnOnOLED() {
    oledOn = true;
    oledTimer = millis();
    Serial.println("OLED turned ON.");
}

// **Function to Turn OLED OFF**
void turnOffOLED() {
    oledOn = false;
    display.clearDisplay();
    display.display();
    Serial.println("OLED turned OFF.");
}

void updateOLED() {
    // ✅ Process OLED toggle request immediately
    if (oledToggleRequested) {
        oledToggleRequested = false;  // ✅ Clear flag
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

        // Convert temperature to Fahrenheit
        float temperatureF = (temperature * 9 / 5) + 32;
        float altitudeFt = altitude * 3.28084;

        // Temperature
        display.setCursor(0, 0);
        display.printf("Temp: %.1f C / %.1f F\n", temperature, temperatureF);

        // Humidity
        display.setCursor(0, 10);
        display.printf("Humidity: %.1f%%\n", humidity);

        // Altitude
        display.setCursor(0, 20);
        display.printf("Alt: %.0f m / %.0f ft\n", altitude, altitudeFt);

        // Pressure
        display.setCursor(0, 30);
        display.printf("Pressure: %.0f hPa\n", pressure);

        // **Display MQTT Sent Message Only for 5 Seconds**
        if (millis() - mqttSentDisplayTime <= 5000) {
            display.setCursor(0, 40);
            display.println("MQTT Sent!");
        }

        // **Display Timestamp at the Bottom**
        updateTimeString();  // ✅ Ensure latest time before displaying
        display.setCursor(0, 54);
        display.println(getTimeString());  // ✅ Now only displays the timestamp

        // **Auto Shutoff After 5 Minutes**
        if (millis() - oledTimer >= 5 * 60 * 1000) { // ✅ 5 minutes timeout
            turnOffOLED();
        }

        display.display();
    } else {
        display.clearDisplay();
        display.display();
    }
}