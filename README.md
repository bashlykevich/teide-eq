# TEIDE EQ: ESP32 Audio Spectrum Visualizer  
**SSD1309 OLED + I2S Microphone + Rotary Encoder**

A compact real-time audio spectrum visualizer using **ESP32-S3**, a **2.42" SSD1309 OLED**, an **I2S microphone**, and a **rotary encoder** for sensitivity control.  
Designed for vinyl stands, desk displays, and ambient music visuals.

---

## ✨ Features

- Real-time FFT-based audio spectrum display  
- 24-bar visualization with smoothing + peak hold  
- Adjustable sensitivity via rotary encoder (KY-040)  
- OLED rendering using U8g2 library  
- I2S digital microphone input (INMP441 / MAX9814 I2S)  
- Startup splash screen  
- Fully customizable visualization logic  

---

## 📦 Hardware List

| Component | Model |
|----------|--------|
| **Microcontroller** | ESP32-S3 DevKitC-1 (N16R8 recommended) |
| **Display** | 2.42" OLED (SSD1309, 128×64, I²C) |
| **Microphone** | INMP441 or MAX9814 (I²S mode) |
| **Rotary Encoder** | KY-040 |
| **Cables** | Dupont jumper wires |
| **Power** | USB-C |

---

## 🔌 Wiring

### OLED (SSD1309, I²C)

| OLED Pin | ESP32-S3 Pin |
|----------|--------------|
| VCC | 5V |
| GND | GND |
| SCL | GPIO **14** |
| SDA | GPIO **15** |
| RES | Not connected |

---

### I²S Microphone

```cpp
#define I2S_WS  15   // LRCLK
#define I2S_SCK 14   // BCLK
#define I2S_SD  32   // Data
```
| Microphone Pin | ESP32-S3 Pin |
|----------------|--------------|
| WS / L/R CLK | 15 |
| SCK / BCLK | 14 |
| SD (Data) | 32 |
| VCC | 3.3V |
| GND | GND |

---

### Rotary Encoder (KY-040)

| Encoder Pin | ESP32-S3 Pin |
|-------------|--------------|
| CLK | 35 |
| DT | 34 |
| SW (Button) | 33 |
| VCC | 3.3V |
| GND | GND |

---

## 🛠 Developer Quick start

### Installing the Development Environment

#### 1️⃣ Install Arduino IDE

https://www.arduino.cc/en/software

#### 2️⃣ Install Git

https://git-scm.com/downloads

#### 3️⃣ Add ESP32 Board Support

Arduino IDE → Preferences → Additional Boards URLs:

https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

Then install:
Tools → Board → Boards Manager → search “ESP32” → Install

---

### 📁 Cloning the Repository

git clone https://github.com/<your-repo>.git
cd <your-repo>

Open:
main.ino in Arduino IDE

---

### ⚙️ Compile & Upload

Select the Board:

Tools → Board → ESP32S3 Dev Module

Recommended settings:
```
Setting	Value
USB CDC On Boot	Enabled
CPU Frequency	240 MHz
USB Mode	CDC Only
Flash Size	16MB (if available)
Upload Speed	921600
```

### Upload Normally

Connect via USB-C and click Upload.

### If Upload Fails

Hold BOOT, click Upload, release when “Connecting…” appears.

---

### 🧪 Serial Debugging

Open:
Tools → Serial Monitor → 115200 baud

You’ll see:
	•	encoder rotation
	•	sensitivity levels
	•	optional FFT logs

---

## 🛡 License

MIT License — free to modify & build upon.

---

## 🙌 Credits

Made with ESP32-S3, FFT, I²S, and a lot of tinkering.
Inspired by classic spectral visualizers (Winamp, VLC, LED panels).