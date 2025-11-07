
# 💧 Smart Water Tank Monitor (ESP8266 + Blynk + Cloudflare Worker)

A smart IoT-based water monitoring system that measures **water level & distance**, sends data securely to a **Cloudflare Worker**, logs values, supports **OTA**, and displays **live stats on GitHub** using Shields.io badges.

## 📊 Live Status

| Water Level | Distance |
|-------------|----------|
| ![Water Level](https://img.shields.io/endpoint?url=https://water-tank-monitor-worker.srinivasa.dev/badge/level&style=flat&labelColor=000000) | ![Distance](https://img.shields.io/endpoint?url=https://water-tank-monitor-worker.srinivasa.dev/badge/distance&style=flat&labelColor=000000) |

## 🚀 Features

- ✅ Real-time Water Level & Distance Monitoring  
- ✅ ESP8266 + JSN-SR04T Ultrasonic Sensor  
- ✅ Deep Sleep for Low Power + OTA Updates  
- ✅ Cloudflare Worker Backend (Data + Logs + API)  
- ✅ Live GitHub README Badges for Level & Distance  
- ✅ Public API Endpoints for Dashboard  

## 📁 Project Structure

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

## 🔧 Hardware Used

| Component | Description |
|-----------|-------------|
| ESP8266   | NodeMCU |
| JSN-SR04T | Waterproof Ultrasonic Sensor |
| Power     | 18650 + 5V adapter |
| Optional  | Relay for motor, OLED, etc. |

## 🛠 Setup & Secrets

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

## 📡 How Data Flows

```
Ultrasonic Sensor → ESP8266 → WiFi → Cloudflare Worker → GitHub README Badges
                                          ↓
                                       (Logs Stored)
```

## ✅ TODO / Future Ideas

- [ ] Add battery voltage monitoring  
- [ ] Auto motor control using relay  
- [ ] Telegram / WhatsApp alerts  
- [ ] Flutter mobile dashboard  

## 👨‍💻 Author

**Designed & Developed by Srinivasa**  
🌐 Portfolio: https://srinivasa.dev  
📦 GitHub: https://github.com/srinivasa-dev

## 📄 License

MIT License — free to use, modify, and improve.
