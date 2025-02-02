# ESP32 Weather Station with FreeRTOS, DHT11, BMP180 & OLED Display

<p align="center">
  <img src="https://i.imgur.com/6BIa1wq.jpeg" alt="ESP32 Weather Station" width="512" height="512">
  <img src="https://i.imgur.com/H0yqByr.png" alt="ESP32 Sensor Close-up" width="658" height="506">
</p>

## Overview
This project is an **ESP32-based Weather Station** that collects temperature, humidity, pressure, and altitude data using **DHT11 and BMP180 sensors**. Data is displayed on an **OLED screen** and sent via **MQTT** to a home automation system like **Home Assistant**. The system is designed using **FreeRTOS tasks** for modularity and efficiency.

## Features
✅ **Temperature, Humidity, Pressure, and Altitude Monitoring**
✅ **OLED Display with Auto Shutoff (5 min timeout)**
✅ **Boot Button (GPIO 0) Toggles OLED ON/OFF**
✅ **MQTT Publishing Every Minute**
✅ **Home Assistant Auto-Discovery (Only on Boot)**
✅ **Persistent MQTT Connection (Prevents Unnecessary Reconnection)**
✅ **Time Synchronization via NTP (Adjustable Timezone)**
✅ **FreeRTOS for Efficient Task Management**

## Hardware Components
| Component              | Description                          |
|------------------------|--------------------------------------|
| ESP32 Board           | Wi-Fi-enabled microcontroller       |
| DHT11 Sensor          | Measures **humidity**               |
| BMP180 Sensor         | Measures **temperature, altitude & pressure** |
| SSD1306 OLED Display  | 0.96-inch I2C OLED screen           |
| MQTT Broker (e.g., Mosquitto) | Receives & processes sensor data |

## Configuration & Adjustments
### **1️⃣ Timezone Adjustment** (Modify in `time_manager.cpp`)
By default, the system is set to **Pacific Standard Time (PST)**:
```cpp
const long gmtOffset_sec = -8 * 3600;    // ✅ Adjust for your timezone
const int daylightOffset_sec = 3600;     // ✅ Adjust for daylight savings
```
Modify these values to match your local timezone.

### **2️⃣ Sensor Offsets for Calibration** (Modify in `bmp180_sensor.cpp`)
If your sensor readings are slightly off, you can adjust the offsets:
```cpp
float tempOffset = 0.0;     // ✅ Adjust for sensor drift
float pressureOffset = 0.0; // ✅ Adjust for barometric calibration
float altitudeOffset = 0.0; // ✅ Adjust for known altitude difference
```
Modify these values based on reference readings.

## FreeRTOS Task Structure
The system uses **FreeRTOS tasks** to manage various functions:
| **Task**               | **Function**                        |
|------------------------|------------------------------------|
| `wifiTask()`          | Manages Wi-Fi connection & reconnection |
| `dhtTask()`          | Reads **humidity** from DHT11       |
| `bmpTask()`          | Reads **temperature, pressure, altitude** from BMP180 |
| `mqttTask()`         | Publishes sensor data via MQTT every **1 minute** |
| `oledTask()`         | Updates OLED display & handles auto shutoff |
| `serialOutputTask()` | Prints sensor data to **Serial Monitor** every **15 seconds** |
| `buttonTask()`       | Toggles OLED ON/OFF when boot button is pressed |

## MQTT Configuration & Topics
The system publishes sensor data to MQTT topics every **1 minute**.
| **Topic**                     | **Description**                    |
|-------------------------------|------------------------------------|
| `homeassistant/sensor/esp32_temperature/state` | Temperature in Fahrenheit |
| `homeassistant/sensor/esp32_humidity/state`    | Humidity percentage |
| `homeassistant/sensor/esp32_pressure/state`    | Pressure in hPa |
| `homeassistant/sensor/esp32_altitude/state`    | Altitude in feet |
| `weather/data`                 | JSON object containing all sensor readings |

### **Sample MQTT Payload (weather/data)**
```json
{
  "temperature": 70.3,
  "humidity": 40.0,
  "pressure": 1000,
  "altitude": 383
}
```

## OLED Display Behavior
- **Displays sensor data & timestamp**
- **Shows "MQTT Sent!" for 5 seconds after publishing**
- **Auto shuts off after 5 minutes of inactivity**
- **Can be toggled ON/OFF via the boot button**

## Installation & Setup
1. **Install Required Libraries** in Arduino IDE:
   - `WiFi.h`
   - `PubSubClient`
   - `Adafruit GFX`
   - `Adafruit SSD1306`
   - `Adafruit BMP085`
   - `DHT sensor library`
2. **Configure `secrets.h` with Wi-Fi & MQTT Credentials**
3. **Compile and Upload to ESP32**
4. **Run MQTT Subscriber Command to Verify Data:**
```sh
mosquitto_sub -h $BrokerIP -u $BrokerUser -P $UserPass! -t "#" -v
```

## Troubleshooting
### **2️⃣ MQTT Messages Not Showing?**
- Run the MQTT subscriber command above to check logs
- Ensure ESP32 is **connected to Wi-Fi**
- Check if MQTT broker is **online**

### **3️⃣ Time is Incorrect?**
- Adjust **timezone settings** in `time_manager.cpp`
- Ensure ESP32 has an **active internet connection**

---
🚀 **Your ESP32 Weather Station is now fully functional!** Enjoy real-time environmental data with MQTT & OLED integration! 🚀


