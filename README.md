# ESP32-C6 Cyberpunk Scroll Wheel

This project transforms a Waveshare ESP32-C6-Touch-LCD-1.47 development board into a completely wireless, standalone Bluetooth Low Energy (BLE) USB mouse scroll wheel.

It features a custom "cyberpunk" aesthetic UI that reacts to gestures and provides dynamic visual feedback as you swipe. No companion software or drivers are required on your host device (Mac/PC).

## Hardware Setup
- **Board:** Waveshare ESP32-C6-Touch-LCD-1.47
- **Display:** 1.47-inch IPS LCD (ST7789 driver)
- **Touch Controller:** Capacitive Touch (AXS5106L via I2C)
- **MCU:** ESP32-C6 (RISC-V, 160MHz)

## Software Stack
- **Framework:** Arduino Core for ESP32 (v3.1.x+)
- **Build Environment:** [PlatformIO](https://platformio.org/)
- **Display Rendering:** moononournation/GFX Library for Arduino
- **BLE HID:** wakwak-koba/ESP32-NimBLE-Mouse (A NimBLE port suitable for ESP32-C6)

## Getting Started

1. **Clone the Repository:**
   ```bash
   git clone https://github.com/mc71/ESP-32-touch-mouse.git
   cd ESP-32-touch-mouse
   ```

2. **Open the Project:**
   Open the folder in VS Code with the PlatformIO extension installed. The `platformio.ini` file will automatically download all required libraries and toolchains.

3. **Build and Upload:**
   Plug in your ESP32-C6 board via USB-C. Click the PlatformIO "Upload" button (or run `pio run -t upload` in the terminal).

4. **Pair the Device:**
   - Once flashed, the screen will boot to a `SYS: STANDBY` state. 
   - Open your computer's Bluetooth settings and look for **"CyberScroll"**. 
   - Connect to it (it acts as a standard BLE Mouse).
   - Once connected, the interface will switch to `SYS: ONLINE` and the scroll deck will activate.

5. **Scroll Away:**
   Swipe vertically on the screen to scroll exactly as you would on a trackpad or mouse wheel. The neon UI bars will animate based on the speed and direction of your scroll input.

## Features
- **Plug and Play:** Emulates a generic BLE HID Mouse. Connects effortlessly without special drivers.
- **Aggressive Power Efficiency:** Built on the modern NimBLE Bluetooth stack, meaning lower RAM and battery usage than legacy ESP32 Bluedroid implementations.
- **Smooth Logic:** Detects scroll velocity and scales the wheel output for snappy interactions.
- **Cyberpunk UI:** Dark mode by default, high-contrast neon highlights (cyan, magenta), tech grids, and a visual movement deck.

## Notice
The standard `t-vk/ESP32 BLE Mouse` library is incompatible with the ESP32-C6 because the C6 relies entirely on the NimBLE bluetooth stack, whereas legacy ESP32s use Bluedroid. This project successfully utilizes a NimBLE-specific mouse fork (`wakwak-koba/ESP32-NimBLE-Mouse`) alongside Espressif's newer Arduino v3+ Core.
