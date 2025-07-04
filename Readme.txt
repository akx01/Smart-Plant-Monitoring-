# 🌱 Smart Plant Monitoring System

An IoT-based smart plant monitoring system using **ESP32**, **Soil Moisture Sensor**, **DHT22**, **Relay + Water Pump**, and **Telegram Bot** for real-time notifications and automated watering.

---

## 📦 Components Used
- ESP32
- DHT22 Temperature & Humidity Sensor
- Soil Moisture Sensor (Analog)
- Relay Module
- 12V Mini Water Pump
- Jumper wires, Breadboard
- Telegram Bot API

---

## 📷 System Images

![Setup](images/setup.jpg)
![Circuit](images/circuit_diagram.jpg)

---

## 📐 Circuit Diagram



---

## 💻 How It Works

1. The **Soil Moisture Sensor** reads the soil level.
2. If the soil is **dry**, the relay turns **ON** and activates the **water pump**.
3. **DHT22** reads temperature & humidity.
4. A **Telegram Bot** sends messages like:
   - 🌡 Temperature: 28°C
   - 💧 Soil Moisture: 40%
   - 🚿 Pump Turned ON/OFF
5. The ESP32 sends regular updates.

---

## 🔧 Installation

1. Clone this repo:
   ```bash
   git clone https://github.com/YOUR_USERNAME/Smart-Plant-Monitoring-System.git
