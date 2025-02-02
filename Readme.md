# ESP32 Weather Station with FreeRTOS, DHT11, BMP180 & OLED Display

<p align="center">
  <img src="https://i.imgur.com/6BIa1wq.jpeg" alt="ESP32 Weather Station" width="512" height="512">
  <img src="https://i.imgur.com/H0yqByr.png" alt="ESP32 Sensor Close-up" width="658" height="506">
</p>

## Overview

This project is an **ESP32-based Weather Station** that collects temperature, humidity, pressure, and altitude data using **DHT11 and BMP180 sensors**. Data is displayed on an **OLED screen** and sent via **MQTT** to a home automation system like **Home Assistant**. The system is designed using **FreeRTOS tasks** for modularity and efficiency.

## Features

✅ **Temperature, Humidity, Pressure, and Altitude Monitoring**\
✅ **OLED Display with Auto Shutoff (5 min timeout)**\
✅ **Boot Button (GPIO 0) Toggles OLED ON/OFF**\
✅ **MQTT Publishing Every Minute**\
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
│   ├── 📄 bmp180_sensor.h      # BMP180 sensor interface with calibration
│   ├── 📄 mqtt_client.h        # MQTT connection management
│   ├── 📄 mqtt_publisher.h     # MQTT message publishing
│   ├── 📄 oled_display.h       # OLED display control
│   ├── 📄 time_manager.h       # NTP time synchronization
│   ├── 📄 secrets.h            # Wi-Fi & MQTT credentials (template included but must be updated)
└── 📺 src                      # Source files implementing component logic
    ├── 📄 wifi_manager.cpp     # Handles Wi-Fi connection logic
    ├── 📄 dht_sensor.cpp       # Implements DHT11 sensor reading
    ├── 📄 bmp180_sensor.cpp    # Implements BMP180 sensor reading with smoothing
    ├── 📄 mqtt_client.cpp      # Manages MQTT connections and subscriptions
    ├── 📄 mqtt_publisher.cpp   # Formats and sends sensor data via MQTT
    ├── 📄 oled_display.cpp     # Updates OLED display and manages auto shutoff
    ├── 📄 time_manager.cpp     # Synchronizes system time via NTP
```

## Secrets Configuration (`secrets.h`)

Edit the `secrets.h` file in the `include` directory with your SSID/Pass & MQTT IP/User/Pass:

```cpp
#ifndef SECRETS_H
#define SECRETS_H

#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASSWORD "your_wifi_password"
#define MQTT_SERVER "your_mqtt_broker_ip"
#define MQTT_USER "your_mqtt_username"
#define MQTT_PASS "your_mqtt_password"

#endif
```

**Do not commit `secrets.h` to version control!**

## Troubleshooting

### **1️⃣ Basic Debugging & Serial Monitor**

Before troubleshooting specific issues, connect your **ESP32 to a PC via USB** and open the **Arduino IDE Serial Monitor** to check real-time logs:

1. **Open Arduino IDE** → Select the correct **board & port**.  
2. **Go to `Tools` → `Serial Monitor`**.  
3. **Set baud rate** to `115200` (or the project’s configured baud rate).  
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

### **3️⃣ Time is Incorrect?**

- Adjust the **timezone settings** in `time_manager.cpp`:
  ```cpp
  const long gmtOffset_sec = -8 * 3600;  // Update for your timezone
  const int daylightOffset_sec = 3600;   // Set to 0 if no daylight savings
  configTime(gmtOffset_sec, daylightOffset_sec, "pool.ntp.org");
  ```
- Ensure ESP32 has an **active internet connection** (NTP sync requires Wi-Fi).  
- Open **Arduino IDE → Tools → Serial Monitor** to check if time synchronization is failing.  
- Restart ESP32 after making changes to apply the new time settings.  


### **4️⃣ OLED Button Delay**

- The button **does work**, but the OLED updates **only every 3 seconds**
- If you press the button, **wait up to 3 seconds** before the screen reacts

🚀 **Your ESP32 Weather Station is now fully functional!** Enjoy real-time environmental data with MQTT & OLED integration! 🚀
