#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "include/dht_sensor.h"
#include "include/bmp390_sensor.h"
#include "include/mqtt_publisher.h"
#include "include/time_manager.h"

// OLED Display Configuration
#define SCREEN_WIDTH 128          // Width of the OLED screen in pixels
#define SCREEN_HEIGHT 64          // Height of the OLED screen in pixels
#define OLED_RESET -1             // Reset pin for OLED (not used with SSD1306)
#define SCREEN_ADDRESS 0x3C       // I2C address for SSD1306 display

// External OLED display instance
extern Adafruit_SSD1306 display;

// OLED State Management
extern bool oledOn;                   // Flag to indicate if OLED is active
extern unsigned long oledTimer;       // Timer for OLED auto-shutoff
const unsigned long oledTimeout = 5 * 60 * 1000;  // 5 minutes timeout in milliseconds

// MQTT Send Notification Timing
extern unsigned long mqttSentDisplayTime;  // Timestamp for when MQTT data was last sent

// Function declarations for OLED operations
void setupOLED();                     // Initializes OLED display
void updateOLED();                    // Updates content on OLED screen
void turnOffOLED();                   // Turns off the OLED screen
void turnOnOLED();                    // Turns on the OLED screen

#endif // OLED_DISPLAY_H
