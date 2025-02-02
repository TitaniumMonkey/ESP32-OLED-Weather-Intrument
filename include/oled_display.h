#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "include/dht_sensor.h"
#include "include/bmp180_sensor.h"
#include "include/mqtt_publisher.h"
#include "include/time_manager.h"

// OLED Display Config
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C  // Default I2C address for SSD1306

extern Adafruit_SSD1306 display;  // ✅ Declare OLED display globally

// OLED Auto Shutoff
extern bool oledOn;
extern unsigned long oledTimer;
const unsigned long oledTimeout = 5 * 60 * 1000; // 5 minutes in milliseconds

// MQTT Sent Message Timing
extern unsigned long mqttSentDisplayTime;

// OLED Functions
void setupOLED();
void updateOLED();
void turnOffOLED();
void turnOnOLED();

#endif // OLED_DISPLAY_H