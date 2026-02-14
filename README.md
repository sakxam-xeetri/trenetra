# 🎥 TRINETRA - ESP32-CAM Surveillance System

![Version](https://img.shields.io/badge/version-1.0-orange)
![Platform](https://img.shields.io/badge/platform-ESP32--CAM-blue)
![License](https://img.shields.io/badge/license-MIT-green)

A professional, mobile-first surveillance system for ESP32-CAM with a modern dark-themed web interface. **Trinetra** (meaning "three eyes" in Sanskrit) provides real-time MJPEG video streaming, photo capture, and SD card storage capabilities through a responsive web UI accessible via dual Wi-Fi modes (Station+AP) - enjoy internet access while viewing your camera!

---

## ✨ Features

### 🎬 **Live Video Streaming**
- **MJPEG streaming** on port 81 with minimal latency
- Dual frame buffer support with PSRAM optimization
- Dynamic resolution switching (QQVGA to SXGA)
- Adjustable JPEG quality (4-63)
- Real-time FPS monitoring

### 📸 **Photo Capture**
- **Instant capture** and download to browser
- **Save to microSD card** with auto-incrementing filenames (`trinetra_00001.jpg`)
- Graceful SD card detection and error handling
- LED flash support during capture

### 🎨 **Modern Web Interface**
- **Dark/Stealth theme** with orange accent highlights
- **Mobile-first responsive design** - works perfectly on phones, tablets, and desktops
- Live stream viewer with status indicators
- Toast notifications for all actions
- Real-time resolution and quality controls
- Flash LED toggle with visual indicator

### 📡 **WiFi Manager (No Code Changes Needed!)**
- **Web-based WiFi configuration** - no need to edit code
- **Scan available networks** - see all WiFi networks with signal strength
- **One-click connection** - select and connect to any network
- **Support for open/free WiFi** - connects to unsecured networks
- **Credentials saved to flash** - survives reboots
- **Auto-reconnect** - reconnects automatically if WiFi drops
- **Reset option** - clear saved credentials anytime
- **Fallback to AP mode** - always works even without WiFi

### 📡 **Dual Wi-Fi Mode (STA+AP)**
- **Station Mode**: Connects to your home/office WiFi for internet access
- **Access Point Mode**: Broadcasts its own "Trinetra" network
- Default SSID: **`Trinetra`**
- Default Password: **`88888888`**
- **Super Simple IP: `1.2.3.4`** (easy to remember!)
- **Hostname: `trinetra.local`** (mDNS support)
- **Auto-redirect**: Captive portal opens web interface automatically
- **Internet + Camera**: Devices on Trinetra WiFi get internet access too!
- **Multiple access methods**: Access via home WiFi OR Trinetra AP
- **OTA Updates**: Wireless firmware updates (password: `trinetra123`)
- No router required - works standalone or networked

### ⚡ **Performance Optimized**
- PSRAM utilization for smooth streaming
- Frame grabbing optimization (`CAMERA_GRAB_LATEST`)
- Minimal memory footprint
- Clean, commented, maintainable code

### 📊 **System Monitoring & Network**
- **Real-time stats dashboard** with auto-refresh
- **Uptime tracking** - Days, hours, minutes
- **ESP32 temperature** monitoring (Celsius/Fahrenheit)
- **Live FPS counter** - Current frame rate
- **Total frames** streamed counter
- **WiFi signal strength** (RSSI in dBm)
- **Connected clients** counter
- **Memory usage** - Heap and PSRAM utilization
- **mDNS hostname** - Access via `trinetra.local`
- **OTA updates** - Wireless firmware upload

---

## 🛠️ Hardware Requirements

### **Required**
- **ESP32-CAM** (AI-Thinker module recommended)
- **OV2640 camera** (included with ESP32-CAM)
- **PSRAM** (required for optimal performance)
- **USB-to-Serial adapter** (FTDI, CP2102, or CH340G) for programming
- **5V power supply** (minimum 500mA recommended)

### **Optional**
- **microSD card** (for photo storage)
- **External antenna** (for better Wi-Fi range)

### **Pin Configuration (AI-Thinker)**
The project uses the standard AI-Thinker ESP32-CAM pinout:
- Camera data pins: GPIO 5, 18, 19, 21, 35, 34, 39, 36
- Flash LED: GPIO 4
- SD Card: GPIO 14, 15, 2, 13 (1-bit mode)

---

## 📦 Installation

### **1. Arduino IDE Setup**

#### Install ESP32 Board Support
1. Open Arduino IDE
2. Go to **File → Preferences**
3. Add to **Additional Board Manager URLs**:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
4. Go to **Tools → Board → Boards Manager**
5. Search for **ESP32** and install **esp32 by Espressif Systems**

#### Select Board & Partition Scheme
1. **Board**: `AI Thinker ESP32-CAM`
2. **Partition Scheme**: `Huge APP (3MB No OTA/1MB SPIFFS)` or any scheme with at least 3MB APP space
3. **PSRAM**: `Enabled`

### **2. Clone/Download Project**
```bash
git clone https://github.com/yourusername/trinetra.git
cd trinetra
```

Or download and extract the ZIP file.

### **3. Upload to ESP32-CAM**

> **Note:** WiFi is configured through the web interface after upload - no code changes needed!

#### Wiring for Programming
| FTDI Adapter | ESP32-CAM |
|--------------|-----------|
| 5V           | 5V        |
| GND          | GND       |
| TX           | U0R (RX)  |
| RX           | U0T (TX)  |

**IMPORTANT**: 
- Connect **GPIO 0 to GND** to enter flash mode
- Disconnect GPIO 0 from GND after upload
- Press **RST** button after upload to run

#### Upload Steps
1. Open `trinetra.ino` in Arduino IDE
2. Select correct **Port** under **Tools → Port**
3. Click **Upload** button
4. Wait for compilation and upload (~1-2 minutes)
5. Disconnect GPIO 0, press RST button

---

## 🚀 Usage

### **1. Power On**
- Power the ESP32-CAM with 5V supply
- Wait 10-15 seconds for boot and AP initialization
- The camera will try to connect to any saved WiFi network
- If no network saved, it will run in AP-only mode (still usable!)

### **2. First Time WiFi Setup**

1. Connect your phone/laptop to **`Trinetra`** WiFi (password: `88888888`)
2. Browser opens automatically at `http://1.2.3.4`
3. Scroll down to **WiFi Setup** section
4. Click **📡 Configure WiFi**
5. Click **🔍 Scan Networks** - shows all available WiFi networks
6. **Select your network** from the list:
   - 🔒 Secured networks - enter password
   - 🔓 Open/free networks - no password needed
7. Click **🔗 Connect**
8. Done! Credentials are saved and survive reboots

### **3. Access Methods (You have TWO options!)**

#### **Option A: Via Your Home WiFi** (Recommended - keeps internet!)
1. Make sure your device (phone/laptop) is connected to your **home WiFi**
2. Open browser and go to the IP shown in Serial Monitor, or use:
   ```
   http://trinetra.local
   ```
3. ✅ You keep internet access while viewing camera!
4. ✅ Everyone on your home network can access the camera

#### **Option B: Direct Connection to Trinetra AP**
1. Look for **`Trinetra`** in your Wi-Fi networks
2. Connect using password: **`88888888`**
3. **Auto-Opens**: Web interface opens automatically (captive portal)
4. If not auto-opened, manually navigate to:
   ```
   http://1.2.3.4
   ```
5. ✅ Your device will have internet through the camera!

### **3. Web Interface URLs**

**Easy to remember addresses:**
```
http://1.2.3.4          (via Trinetra WiFi)
http://trinetra.local   (via any network with mDNS)
http://192.168.x.x      (via home WiFi - check Serial Monitor)
```

**Direct stream access:**
```
http://1.2.3.4:81/stream
```

### **4. Controls**

#### Stream Control
- **▶ Start** - Begin live MJPEG streaming
- **■ Stop** - Stop streaming
- **📷 Capture** - Take photo and download to browser
- **💾 Save SD** - Capture photo and save to SD card

#### Camera Settings
- **Resolution Dropdown** - Select from QQVGA (160×120) to SXGA (1280×1024)
  - Higher resolution = slower FPS but better quality
  - Recommended: QVGA or VGA for smooth streaming
- **Quality Slider** - Adjust JPEG quality (4=best, 63=worst)
  - Lower value = better quality, larger file size
  - Recommended: 10-15 for balanced performance
- **Brightness Slider** - Adjust image brightness (-2 to +2)
  - Negative = darker, Positive = brighter
  - Default: 0
- **Contrast Slider** - Adjust image contrast (-2 to +2)
  - Negative = lower contrast, Positive = higher contrast
  - Default: 0

#### Flash LED
- **💡 ON** - Turn on built-in flash LED
- **OFF** - Turn off flash LED
- Visual indicator shows current LED state

#### System Monitor 📊 **NEW!**
Real-time system statistics auto-refreshing every 3 seconds:
- **⏱️ Uptime** - How long the camera has been running
- **🌡️ Temperature** - ESP32 chip temperature in Celsius
- **🎥 FPS** - Current streaming frame rate
- **📊 Frames** - Total frames streamed since boot
- **📡 Signal** - WiFi signal strength (RSSI)
- **👥 Clients** - Number of connected devices
- **💾 Heap** - RAM memory usage and percentage
- **⚡ PSRAM** - PSRAM usage and percentage
- **🔄 Refresh** - Manual refresh button

---

## 📂 Project Structure

```
trenetra/
├── trenetra.ino          # Main Arduino sketch (setup, Wi-Fi AP, camera init)
├── app_httpd.cpp         # HTTP server & request handlers
├── camera_index.h        # Embedded HTML/CSS/JS web interface
├── board_config.h        # Board selection (AI-Thinker)
├── camera_pins.h         # Pin definitions for various ESP32-CAM models
├── partitions.csv        # Partition table (optional)
├── ci.yml                # CI configuration (optional)
└── README.md             # This file
```

---

## 🔧 Configuration

### **OTA (Over-The-Air) Updates** 🆕
Update firmware wirelessly without USB cable!

1. In Arduino IDE, go to **Tools → Port**
2. Select **Network Ports** → **trinetra at 1.2.3.4**
3. Click **Upload** (will ask for password: `trinetra123`)
4. Wait for upload to complete
5. Device will auto-reboot with new firmware

**To change OTA password**, edit in `trinetra.ino`:
```cpp
ArduinoOTA.setPassword("your_password_here");
```

### **Change Access Point Settings**
Edit in `trinetra.ino`:
```cpp
const char *ap_ssid     = "YourCustomSSID";
const char *ap_password = "YourPassword";  // Min 8 chars, or "" for open AP
```

### **Adjust Default Camera Settings**
Edit in `trinetra.ino` → `setup()`:
```cpp
config.frame_size   = FRAMESIZE_QVGA;     // QQVGA, QVGA, VGA, SVGA, XGA, SXGA, UXGA
config.jpeg_quality = 12;                  // 0-63 (lower = better)
config.fb_count     = 2;                   // 1 or 2 (dual buffer needs PSRAM)
```

### **Enable/Disable SD Card**
SD card is auto-detected. If not present, save-to-SD will show error message. No code changes needed.

### **LED Flash Brightness**
Edit in `app_httpd.cpp`:
```cpp
#define CONFIG_LED_MAX_INTENSITY 255  // 0-255
```

---

## 🐛 Troubleshooting

### **Camera Init Failed**
- **Check**: PSRAM is enabled in Arduino IDE settings
- **Check**: Partition scheme has at least 3MB APP space
- **Check**: Camera ribbon cable is properly connected
- **Fix**: Auto-reboot after 3 seconds

### **Video Stream Not Loading**
- **Check**: Connected to `Trinetra_AP` Wi-Fi
- **Check**: Accessing `http://1.2.3.4` (not HTTPS)
- **Check**: Port 81 is not blocked by firewall
- **Fix**: Click Start button, ensure stream URL is `http://1.2.3.4:81/stream`

### **SD Card Not Detected**
- **Check**: SD card is formatted as FAT32
- **Check**: SD card is properly inserted before power-on
- **Note**: AI-Thinker shares GPIO 4 with flash LED - SD init uses 1-bit mode

### **Can't Upload Sketch**
- **Check**: GPIO 0 is connected to GND during upload
- **Check**: TX/RX are not swapped on FTDI adapter
- **Check**: Correct COM port selected
- **Check**: Board set to "AI Thinker ESP32-CAM"

### **Compile Errors**
- **Error**: `PROGMEM not defined` → ESP32 board support not installed
- **Error**: `Camera init failed` → Wrong board selected or PSRAM disabled
- **Fix**: Update ESP32 board package to latest version

### **Low FPS / Choppy Stream**
- **Reduce** resolution to QVGA or QQVGA
- **Increase** JPEG quality value (15-25)
- **Check** Wi-Fi signal strength
- **Disable** other devices on the AP

---

## 📡 API Endpoints

| Endpoint | Method | Description |
|----------|--------|-------------|
| `/` | GET | Serve web interface |
| `/stream` | GET | MJPEG live stream (port 81) |
| `/capture` | GET | Capture & download single JPEG |
| `/save-photo` | GET | Capture & save to SD (JSON response) |
| `/control?var=X&val=Y` | GET | Set camera parameter |
| `/status` | GET | Get camera status (JSON) |
| `/led?state=0\|1` | GET | Control flash LED |
| `/system-stats` | GET | Get system monitoring stats (JSON) |

### **Example: Change Resolution via API**
```bash
curl "http://1.2.3.4/control?var=framesize&val=8"
# 3=QQVGA, 5=QVGA, 8=VGA, 9=SVGA, 10=XGA, 12=SXGA
```

### **Example: Adjust Brightness**
```bash
curl "http://1.2.3.4/control?var=brightness&val=1"
# Range: -2 to +2
```

### **Example: Capture Photo via API**
```bash
curl "http://1.2.3.4/capture" --output photo.jpg
```

### **Example: Get System Stats**
```bash
curl "http://1.2.3.4/system-stats"
# Returns JSON with uptime, temp, FPS, WiFi, memory stats
```

---

## 🎯 Performance Tips

1. **For Smooth Streaming**: Use QVGA (320×240) with quality 10-12
2. **For High Quality Photos**: Switch to XGA/SXGA before capture
3. **Battery Operation**: Lower resolution and increase quality value to reduce power
4. **Multiple Viewers**: Reduce FPS by increasing quality value
5. **Range Extension**: Use external antenna on ESP32-CAM

---

## 🔐 Security Notes

⚠️ **This is a basic implementation for personal/educational use**

- Default password (`12345678`) should be changed for security
- No encryption on video stream (HTTP, not HTTPS)
- No authentication on web interface
- Suitable for isolated networks or temporary setups

**For production use**, consider adding:
- HTTPS with SSL certificates
- User authentication (Basic Auth or OAuth)
- Access control lists (ACL)
- Encrypted storage for sensitive photos

---

## 📄 License

This project is open-source and available under the **MIT License**.

```
Copyright (c) 2025 Trinetra Project

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
```

---

## 🙏 Credits

- **ESP32-CAM** module by AI-Thinker
- **ESP32 Arduino Core** by Espressif Systems
- **Arduino IDE** community
- Camera drivers and examples from ESP-IDF

---

## 📬 Support

For issues, feature requests, or contributions:
- Open an issue on GitHub
- Submit pull requests
- Share your implementations and modifications

---

## 🚧 Future Enhancements

- [ ] Multiple client streaming support
- [ ] Motion detection with alerts
- [ ] Time-lapse photo capture
- [ ] FTP upload for photos
- [ ] MQTT integration
- [ ] Face detection overlay
- [ ] Recording to SD card (video)
- [ ] Mobile app (Android/iOS)

---

**Built with ❤️ for the maker community**

*Trinetra - Three Eyes Watching*
