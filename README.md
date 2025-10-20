# 🌡️ HTU21 Temperature & Humidity Sensor Example on ESP32-S3-DevKitC-1

This project demonstrates how to read temperature and humidity data from an **HTU21 sensor** using **I²C** on the **ESP32-S3-DevKitC-1**.

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

## 🚀 Getting Started

### 1️⃣ Set Target Chip

Before building, make sure the correct ESP32 target is set:

```bash
idf.py set-target esp32s3
2️⃣ Build & Flash Firmware
Connect your ESP32-S3-DevKitC-1 and run:

bash
Copy code
idf.py -p PORT flash monitor
Replaces PORT with your serial port (e.g., /dev/ttyUSB0 or COM3).

The monitor tool will display temperature and humidity readings in real-time.

To exit the monitor, press Ctrl-].

📊 Example Output
yaml
Copy code
I (303) example: HTU21 sensor initialized successfully
I (313) example: Reading temperature and humidity...
I (323) example: Temperature: 26.06 ℃, Humidity: 45.2 %
I (1323) example: Temperature: 26.08 ℃, Humidity: 45.1 %
I (2323) example: Temperature: 26.10 ℃, Humidity: 45.0 %
I (3323) example: Temperature: 26.12 ℃, Humidity: 44.9 %
I (4323) example: Temperature: 26.14 ℃, Humidity: 44.9 %
🔧 Troubleshooting
Ensure SDA and SCL pins are correctly connected.

Check that the sensor has power (VCC & GND).

If I²C address conflicts occur, run an I²C scan utility.

Verify serial port selection when using the monitor.

📚 References
ESP-IDF Getting Started Guide

HTU21 Sensor Datasheet

yaml
Copy code

---

If you want, I can also **add a simple architecture diagram** showing:

HTU21 → I²C → ESP32-S3-DevKitC-1 → UART → Host PC

vbnet
Copy code
