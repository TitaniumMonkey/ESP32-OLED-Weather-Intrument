# ESP32 Weather Station with OLED Display & MQTT

## Overview
This project uses an **ESP32**, a **DHT11 temperature & humidity sensor**, and an **SSD1306 OLED display** to collect environmental data and send it to an **MQTT broker**. The data is displayed on the OLED screen and published to an MQTT topic every **5 minutes**.

## Features
- Reads temperature & humidity from a **DHT11 sensor**
- Displays real-time data on a **0.96-inch SSD1306 OLED**
- Sends data to an **MQTT broker** every 5 minutes
- Displays a confirmation message when MQTT data is sent
- Uses **Wi-Fi** to connect to the network and syncs time via **NTP**
- **Flips the display** to match the ESP32 OLED module's default layout
- Allows **manual OLED toggling** using the ESP32 **built-in button**

## Components Used
| Component             | Description                   |
|----------------------|-----------------------------|
| ESP32 Board         | Wi-Fi-enabled microcontroller |
| DHT11 Sensor       | Measures temperature & humidity |
| SSD1306 OLED Display | 0.96-inch I2C OLED screen  |
| MQTT Broker (e.g., Mosquitto) | Receives and processes sensor data |

## Required Libraries
Install the following libraries in **Arduino IDE**:

1. **WiFi** (Built-in for ESP32)
2. **PubSubClient** (by Nick O'Leary)
3. **DHT sensor library** (by Adafruit)
4. **Adafruit GFX Library** (by Adafruit)
5. **Adafruit SSD1306** (by Adafruit)

### Installing Libraries
1. Open **Arduino IDE**
2. Go to **Sketch → Include Library → Manage Libraries**
3. Search for and install the required libraries

## Board Setup in Arduino IDE
1. **Install ESP32 Board Support**
   - Open **Arduino IDE**
   - Go to **File → Preferences**
   - In **Additional Board Manager URLs**, add:
     ```
     https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
     ```
   - Go to **Tools → Board → Boards Manager**
   - Search for **ESP32** and install the latest version

2. **Select the Board**
   - Go to **Tools → Board**
   - Select **ESP32 Dev Module**
   - Set Upload Speed: **115200**
   - Flash Mode: **QIO**
   - Partition Scheme: **Default**

## Wiring Diagram
| ESP32 Pin | DHT11 Pin |
|-----------|----------|
| 3.3V      | VCC      |
| GND       | GND      |
| GPIO15    | DATA     |

## Configuration

### Setting Your Timezone
This project uses **NTP (Network Time Protocol)** to sync the correct time. You can adjust the timezone and **enable/disable daylight saving time (DST)** in the following section of the code:

```cpp
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -8 * 3600; // Pacific Standard Time (PST)
const int daylightOffset_sec = 0;     // No DST (Set to 3600 for 1 hour DST adjustment)
```

#### Finding Your Timezone
To get your **GMT offset**, you can check:
- [Time Zone List](https://en.wikipedia.org/wiki/List_of_UTC_time_offsets)
- Running this Linux command:
  ```bash
  timedatectl
  ```
  Look for `Time zone` and `UTC offset`.

#### Adjusting for DST
- If your region **uses DST**, change `daylightOffset_sec` to `3600` (1 hour forward).
- If your region **does not use DST**, keep it as `0`.

Before uploading the code, create a `secrets.h` file next to your `.ino` file with your **Wi-Fi and MQTT credentials**:

```cpp
#ifndef SECRETS_H
#define SECRETS_H

#define WIFI_SSID "Your_WiFi_SSID"
#define WIFI_PASSWORD "Your_WiFi_Password"

#define MQTT_SERVER "Your_MQTT_Broker_IP"
#define MQTT_USER "Your_MQTT_Username"
#define MQTT_PASS "Your_MQTT_Password"

#endif // SECRETS_H
```

## Uploading the Code
1. **Connect the ESP32 to your PC via USB**
2. Open the **Arduino IDE**
3. Select **ESP32 Dev Module** under **Tools → Board**
4. Open **Tools → Port** and select the correct COM port
5. Click the **Upload** button
6. Open the **Serial Monitor** (baud rate: `19200`) to check the logs

## MQTT Data Format
The ESP32 publishes the following message to the MQTT topic:
```plaintext
01/28/25 10:43PM PST | Humidity: 35.0 % | Temperature: 18.2 °C | 64.8 °F
```

## Expected OLED Display Layout
```
Humidity:
  35.0%
Temperature:
  18.2 C
  64.8 F

01/28/25 10:43PM PST
```

## Troubleshooting
### 1. **OLED Display Not Working**
- Ensure **SDA (GPIO21) & SCL (GPIO22)** are correctly connected.
- Check **I2C address** (default is `0x3C`).
- Try scanning I2C devices using an I2C scanner sketch.

### 2. **ESP32 Not Connecting to Wi-Fi**
- Double-check **SSID and Password** in `secrets.h`.
- Ensure your **Wi-Fi network is 2.4 GHz** (ESP32 doesn't support 5 GHz).

### 3. **MQTT Not Connecting**
- Verify the **MQTT broker IP and credentials** in `secrets.h`.
- Ensure the broker is running and accessible.
- Try disabling MQTT authentication temporarily.

## Future Improvements
- Add **OTA updates** for remote firmware upgrades
- Implement **deep sleep** to save power
- Add support for **more sensors (BMP280, BME680, etc.)**

---
### 🚀 Enjoy your ESP32 Weather Station!
Let me know if you have any issues. Feel free to contribute and improve this project! 😊

