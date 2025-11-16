<p align="center">
  <img src="https://water-tank-monitor.srinivasa.dev/assets/banner.jpeg" alt="Smart Water Tank Monitor Banner" width="100%" />
</p>

## 💧 Smart Water Tank Monitor (ESP8266 + Blynk + Cloudflare Worker)

A smart IoT-based water monitoring system that measures **water level & distance**, sends data securely to a **Cloudflare Worker**, logs values, supports **OTA**, and displays **live stats on GitHub** using Shields.io badges.

### 📊 Live Status

🌐 **Live Dashboard** | https://water-tank-monitor.srinivasa.dev |

| Water Level | Distance |
|-------------|----------|
| ![Water Level](https://img.shields.io/endpoint?url=https://water-tank-monitor-worker.srinivasa.dev/badge/level&style=flat&labelColor=000000) | ![Distance](https://img.shields.io/endpoint?url=https://water-tank-monitor-worker.srinivasa.dev/badge/distance&style=flat&labelColor=000000) |

### 🚀 Features

- ✅ Real-time Water Level & Distance Monitoring  
- ✅ ESP8266 + JSN-SR04T Ultrasonic Sensor  
- ✅ OTA Updates  
- ✅ Cloudflare Worker Backend (Data + Logs + API)  
- ✅ Live GitHub README Badges for Level & Distance  
- ✅ Public API Endpoints for Dashboard  

### 📁 Project Structure

```
📦 Smart-Water-Monitor
 ┣ 📁 src
 ┃ ┣ main.ino              # ESP8266 firmware
 ┃ ┣ secrets.h             # WiFi, Blynk, Worker secrets (ignored in git)
 ┣ 📁 cloudflare-worker
 ┃ ┗ index.js              # Cloudflare Worker API logic
 ┣ .gitignore
 ┣ README.md
```

### 🔧 Hardware Used

| Component         | Description                              | Link |
|-------------------|------------------------------------------|------|
| **ESP8266**       | NodeMCU Lua WiFi Dev Board               | 🔗 https://robu.in/product/nodemcu-lua-wifi-iot-development-board-based/ |
| **Ultrasonic Sensor** | JSN-SR04T Waterproof Distance Sensor     | https://robodo.in/products/waterproof-ultrasonic-module-jsn-sr04t-distance-transducer-sensor-for-arduino *(Based on selection)* |
| **18650 Battery** | BAK NMC 2600mAh Lithium-Ion 3.6V         | 🔗 https://robu.in/product/bak-nmc-18650-2600mah-3c-lithium-ion-3-6v-battery/ |
| **Battery Case**  | 18650 USB Power Bank Case (Aluminium)    | 🔗 https://robu.in/product/5v-usb-aluminium-body-power-bank-case-for-18650-battery-black/ |
| **Micro USB Cable** | Micro USB A to Micro B (120cm)            | 🔗 https://robu.in/product/micro-usb-a-to-micro-b-cable-120cm/ |
| **Switch Box**    | Wall Mount Polycarbonate Enclosure       | 🔗 https://www.amazon.in/SEER-Electrical-Switches-Combination-Polycarbonate/dp/B0FBRTFY32 |
| *Optional*        | Relay for motor control, OLED display, etc. | — |

### 💰 Development Cost & Learnings

Building this project involved experimentation, a few wrong turns, and ultimately, a working setup. Here's a transparent look at the cost and learnings:

### 💸 Cost Breakdown

| Item                   | Amount (INR) |
|------------------------|--------------|
| Final components used  | ~₹1000        |
| Incorrect purchases / unused parts | ~₹900 |
| **Total Spent**        | **~₹1,860**  |

### 🛠 Setup & Secrets

Create a file **`src/secrets.h`** (NOT uploaded to GitHub):

```cpp
#define WIFI_SSID "YourWiFi"
#define WIFI_PASS "YourPassword"
#define BLYNK_AUTH "YourBlynkToken"
#define CF_WORKER_ENDPOINT "YourEndpoint"
#define CF_API_KEY "YourSecureAPIKey"
```

Add this to `.gitignore` to keep it private:

```
/src/secrets.h
```

### 📡 How Data Flows

```
Ultrasonic Sensor → ESP8266 → WiFi → Cloudflare Worker → GitHub README Badges
                                          ↓
                                       (Logs Stored)
```

### ⚠️ Current Limitations
- No Deep Sleep Mode
Deep sleep cannot be implemented because the setup relies on a USB power bank, which cuts off power when the ESP8266 is in low-power mode.
- Battery Percentage Unavailable
The current battery setup does not support real-time voltage monitoring, making it difficult to determine the battery level or estimate runtime.

### 💡 Future Ideas

- Add battery voltage monitoring
- Auto motor control using relay
- Telegram / WhatsApp alerts
- Flutter mobile dashboard
- Deep Sleep for low power consumption

### 👨‍💻 Author

**Designed & Developed by Srinivasa**  
🌐 Portfolio: https://srinivasa.dev  
📦 GitHub: https://github.com/srinivasa-dev

### 📄 License

This project is licensed under the **Apache License 2.0**.  
You are free to use, modify, and distribute this software, but you **must include proper attribution** and a copy of the license.

📜 Full License: [Apache 2.0](LICENSE)
