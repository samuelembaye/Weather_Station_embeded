# 🌡️ HTU21 Temperature & Humidity Sensor  ESP32-S3-DevKitC-1

This project demonstrates how to read temperature and humidity data from an **HTU21 sensor** using **I²C** on the **ESP32-S3-DevKitC-1**  then sends the data via UART to be used in REPO Weather_Station.

---

## 🧭 Overview

The ESP32-S3-DevKitC-1 does **not have a built-in temperature sensor**. This firmware reads data from an external HTU21 sensor over I²C and outputs the readings via UART to a host machine for monitoring or further processing.

---

## 🛠 Hardware Required

* **ESP32-S3-DevKitC-1** development board  
* **HTU21 temperature & humidity sensor**  
* Connecting wires (SDA, SCL, VCC, GND)  
* USB cable for power and programming  

**I²C Connection Example:**

| HTU21 Pin | ESP32-S3 Pin |
|------------|--------------|
| VCC        | 3.3V         |
| GND        | GND          |
| SDA        | GPIO21       |
| SCL        | GPIO22       |

> Adjust GPIO pins if needed in your code.

---

## ⚙️ Firmware Features

- Reads temperature (℃) and humidity (%) from HTU21 sensor  
- Sends structured data over UART  
- Supports periodic readings for real-time monitoring  
- Compatible with ESP32-S3-DevKitC-1  

---
