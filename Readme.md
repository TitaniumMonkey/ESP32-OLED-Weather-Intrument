# ESP32 Weather Station with FreeRTOS, DHT11, BMP390 & OLED Display

<p align="center">
  <img src="https://i.imgur.com/xkrLPfS.jpeg" alt="ESP32 Weather Station">
  <img src="https://i.imgur.com/H0yqByr.png" alt="ESP32 Sensor Close-up" width="658" height="506">
</p>

## Overview

This project is an **ESP32-based Weather Station** that collects temperature, humidity, pressure, and altitude data using **DHT11 and BMP390 sensors**. Data is displayed on an **OLED screen** and sent via **MQTT** to a home automation system like **Home Assistant**. The system is designed using **FreeRTOS tasks** for modularity and efficiency.

## Features

✅ **Temperature, Humidity, Pressure, and Altitude Monitoring**\
✅ **OLED Display with Auto Shutoff (5 min timeout)**\
✅ **Boot Button (GPIO 0) Toggles OLED ON/OFF**\
✅ **MQTT Publishing Every Five Minutes**\
✅ **Home Assistant Auto-Discovery (Only on Boot)**\
✅ **Persistent MQTT Connection (Prevents Unnecessary Reconnection)**\
✅ **Time Synchronization via NTP (Adjustable Timezone)**\
✅ **FreeRTOS for Efficient Task Management**

## Project Structure

```
📺 ESP32_Weather_Station
├── 📄 sketch.ino               # Main entry point, initializes FreeRTOS tasks
├── 📄 Readme.md                # Project documentation
├── 📄 LICENSE                  # License file
├── 📺 include                  # Header files for modular components
│   ├── 📄 wifi_manager.h       # Wi-Fi connection handling
│   ├── 📄 dht_sensor.h         # DHT11 sensor interface
│   ├── 📄 bmp390_sensor.h      # BMP390 sensor interface with calibration
│   ├── 📄 mqtt_client.h        # MQTT connection management
│   ├── 📄 mqtt_publisher.h     # MQTT message publishing
│   ├── 📄 oled_display.h       # OLED display control
│   ├── 📄 time_manager.h       # NTP time synchronization
│   ├── 📄 secrets.h            # Wi-Fi & MQTT credentials (template included but must be updated)
└── 📺 src                      # Source files implementing component logic
    ├── 📄 wifi_manager.cpp     # Handles Wi-Fi connection logic
    ├── 📄 dht_sensor.cpp       # Implements DHT11 sensor reading
    ├── 📄 bmp390_sensor.cpp    # Implements BMP390sensor reading with smoothing
    ├── 📄 mqtt_client.cpp      # Manages MQTT connections and subscriptions
    ├── 📄 mqtt_publisher.cpp   # Formats and sends sensor data via MQTT
    ├── 📄 oled_display.cpp     # Updates OLED display and manages auto shutoff
    ├── 📄 time_manager.cpp     # Synchronizes system time via NTP
```


## Required Libraries
Install the following libraries in **Arduino IDE**:

1. **WiFi** (Built-in for ESP32)
2. **PubSubClient** (by Nick O'Leary)
3. **DHT sensor library** (by Adafruit)
4. **Adafruit GFX Library** (by Adafruit)
5. **Adafruit SSD1306** (by Adafruit)
6. **Adafruit BMP3XX Library** (by Adafruit)

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
|   ESP32 Pin  | DHT11 Pin |
|--------------|-----------|
| 3.3V         | VCC       |
| GND          | GND       |
| GPIO4 (D4)   | DATA      |

|  ESP32 Pin   | BMP390 Pin |
|--------------|-----------|
| 3.3V         | VCC       |
| GND          | GND       |
| GPIO21 (D21) | SDA       |
| GPIO22 (D22) | SCL       |
 
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
01/28/25 10:43PM PST | Temp: 18.2 C / 64.8 F | Humidity: 35.0 % | Alt: 72.1 m / 236 ft | Pressure: 999 hPa
```

## Expected OLED Display Layout
```
Temp: 22.5 C / 72.5 F
Humidity: 37.0%
Alt: 72.1 m / 236 ft
Pressure: 999 hPa

02/01/25 05:33PM PST
```

## Troubleshooting

### **1️⃣ Basic Debugging & Serial Monitor**

Before troubleshooting specific issues, connect your **ESP32 to a PC via USB** and open the **Arduino IDE Serial Monitor** to check real-time logs:

1. **Open Arduino IDE** → Select the correct **board & port**.  
2. **Go to `Tools` → `Serial Monitor`**.  
3. **Set baud rate** to `19200` (or the project’s configured baud rate).  
4. Observe the output to check:
   - **Wi-Fi connection status**  
   - **MQTT broker connection logs**  
   - **Sensor readings & system messages**  

If the ESP32 fails to connect to Wi-Fi or MQTT, the error messages will indicate where the issue is.

---

### **2️⃣ MQTT Messages Not Showing?**

- Run the following command to subscribe to **all MQTT messages** and check if data is being published:
  ```sh
  mosquitto_sub -h <MQTT_BROKER_IP_OR_HOST> -u <MQTT_USER> -P <MQTT_PASS> -t "#" -v
  ```
  - Replace `<MQTT_BROKER_IP_OR_HOST>` with your **MQTT broker’s IP address or hostname**  
  - Replace `<MQTT_USER>` and `<MQTT_PASS>` with your **MQTT credentials**  
  - **MQTT should not be left in unauthenticated mode** for security reasons.  

- Ensure ESP32 is **connected to Wi-Fi**  
- Check if MQTT broker is **online and accessible**  

---

🚀 **Your ESP32 Weather Station is now fully functional!** Enjoy real-time environmental data with MQTT & OLED integration! 🚀
