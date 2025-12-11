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

