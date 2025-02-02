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
├── 📄 sketch.ino          # Main entry point, initializes FreeRTOS tasks
├── 📄 Readme.md           # Project documentation
├── 📄 LICENSE             # License file
├── 📺 include             # Header files for modular components
│   ├── 📄 wifi_manager.h     # Wi-Fi connection handling
│   ├── 📄 dht_sensor.h       # DHT11 sensor interface
│   ├── 📄 bmp180_sensor.h    # BMP180 sensor interface with calibration
│   ├── 📄 mqtt_client.h      # MQTT connection management
│   ├── 📄 mqtt_publisher.h   # MQTT message publishing
│   ├── 📄 oled_display.h     # OLED display control
│   ├── 📄 time_manager.h     # NTP time synchronization
│   ├── 📄 secrets.h          # Wi-Fi & MQTT credentials (excluded from version control)
└── 📺 src                   # Source files implementing component logic
    ├── 📄 wifi_manager.cpp   # Handles Wi-Fi connection logic
    ├── 📄 dht_sensor.cpp     # Implements DHT11 sensor reading
    ├── 📄 bmp180_sensor.cpp  # Implements BMP180 sensor reading with smoothing
    ├── 📄 mqtt_client.cpp    # Manages MQTT connections and subscriptions
    ├── 📄 mqtt_publisher.cpp # Formats and sends sensor data via MQTT
    ├── 📄 oled_display.cpp   # Updates OLED display and manages auto shutoff
    ├── 📄 time_manager.cpp   # Synchronizes system time via NTP
```

## Secrets Configuration (`secrets.h`)

Create a `secrets.h` file in the `include` directory with the following format:

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

### **2️⃣ MQTT Messages Not Showing?**

- Run the MQTT subscriber command above to check logs
- Ensure ESP32 is **connected to Wi-Fi**
- Check if MQTT broker is **online**

### **3️⃣ Time is Incorrect?**

- Adjust **timezone settings** in `time_manager.cpp`
- Ensure ESP32 has an **active internet connection**

### **4️⃣ OLED Button Delay**

- The button **does work**, but the OLED updates **only every 3 seconds**
- If you press the button, **wait up to 3 seconds** before the screen reacts
- **Only one press is needed**—do not repeatedly press the button

🚀 **Your ESP32 Weather Station is now fully functional!** Enjoy real-time environmental data with MQTT & OLED integration! 🚀
