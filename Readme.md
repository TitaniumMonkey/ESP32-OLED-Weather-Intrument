# ESP32 Weather Station with OLED Display & MQTT

## Overview

This project utilizes an **ESP32**, a **BMP180 temperature, pressure & altitude sensor**, and an **SSD1306 OLED display** to collect and display environmental data. The data is also published to an **MQTT broker** every **5 minutes** for remote monitoring. Now, **MQTT Auto Discovery** is implemented for seamless integration with **Home Assistant**.

## Features

- **Temperature, Pressure & Altitude Monitoring** using a **BMP180 Sensor**
- **OLED Display** shows real-time data
- **Wi-Fi Connectivity** for NTP time synchronization
- **MQTT Data Publishing** every **5 minutes**, with a timestamp
- **MQTT Auto Discovery** for **Home Assistant Integration**
- **OLED Displays "MQTT Sent!" for 5 seconds when an MQTT message is published**
- **OLED Toggle** via a physical button
- **Smoothed Altitude Measurements** using a moving average filter
- **Automatic Reconnection to MQTT Broker**

## Components Used

| Component                     | Description                                  |
| ----------------------------- | -------------------------------------------- |
| ESP32 Board                   | Wi-Fi-enabled microcontroller               |
| BMP180 Sensor                 | Measures temperature, pressure & altitude   |
| SSD1306 OLED Display          | 0.96-inch I2C OLED screen                   |
| MQTT Broker (e.g., Mosquitto) | Receives and processes sensor data          |

## Required Libraries

Ensure you have the following libraries installed in **Arduino IDE**:

1. **WiFi** (Built-in for ESP32)
2. **PubSubClient** (by Nick O'Leary)
3. **Adafruit BMP085 Library** (by Adafruit, supports BMP180)
4. **Adafruit GFX Library** (by Adafruit)
5. **Adafruit SSD1306** (by Adafruit)
6. **ArduinoJson** (for MQTT payload formatting)

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

| ESP32 Pin | BMP180 Pin |
|-----------|-----------|
| 3.3V      | VCC       |
| GND       | GND       |
| D21       | SDA       |
| D22       | SCL       |

## Configuration

### Setting Your Timezone

The ESP32 syncs time using **NTP (Network Time Protocol)**. Adjust the timezone and **enable/disable daylight saving time (DST)** in the following code:

```cpp
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -8 * 3600; // Pacific Standard Time (PST)
const int daylightOffset_sec = 0;     // No DST (Set to 3600 for 1 hour DST adjustment)
```

#### Finding Your Timezone

To get your **GMT offset**, check:

- [Time Zone List](https://en.wikipedia.org/wiki/List_of_UTC_time_offsets)
- Run this Linux command:
  ```bash
  timedatectl
  ```
  Look for `Time zone` and `UTC offset`.

#### Adjusting for DST

- If your region **uses DST**, set `daylightOffset_sec` to `3600`.
- If your region **does not use DST**, keep it as `0`.

## Secrets File

Before uploading the code, create a `secrets.h` file alongside your `.ino` file containing **Wi-Fi and MQTT credentials**:

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

1. **Connect the ESP32 via USB**
2. Open **Arduino IDE**
3. Select **ESP32 Dev Module** under **Tools → Board**
4. Open **Tools → Port** and select the correct COM port
5. Click **Upload**
6. Open the **Serial Monitor** (baud rate: `19200`) to check the logs

## MQTT Auto Discovery (Home Assistant Integration)

This ESP32 firmware now **automatically sets up sensors in Home Assistant** via MQTT Auto Discovery.

### Topics Used:

| Sensor          | State Topic | Discovery Topic |
|----------------|---------------------------|--------------------------------|
| Temperature (°F) | `homeassistant/sensor/esp32_temperature_F/state` | `homeassistant/sensor/esp32_temperature_F/config` |
| Altitude (ft)   | `homeassistant/sensor/esp32_altitude/state` | `homeassistant/sensor/esp32_altitude/config` |
| Pressure (hPa)  | `homeassistant/sensor/esp32_pressure/state` | `homeassistant/sensor/esp32_pressure/config` |

If Home Assistant restarts, the ESP32 resends discovery topics when it detects HA is back online.

## MQTT Data Format

The ESP32 publishes the following message to the MQTT topic **every 5 minutes**:

```plaintext
01/28/25 10:43PM PST | Temp: 18.2 C / 64.8 F | Alt: 380 m / 1247 ft | Pressure: 995 hPa
```

## OLED Display Layout

```
Temp: 18.2 C / 64.8 F
Alt: 380 m / 1247 ft
Pressure: 995 hPa

MQTT Sent!

01/28/25 10:43PM PST
```

## Troubleshooting

### 1. **OLED Display Not Working**

- Ensure **SDA (D21) & SCL (D22)** are correctly connected.
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

- Implement **deep sleep** to save power
- Add support for **more sensors (BME280, BME680, etc.)**

---

### 🚀 Enjoy your ESP32 Weather Station!

Let me know if you have any issues. Feel free to contribute and improve this project! 😊

