#include "esp_lcd_touch_axs5106l.h"
#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <BleMouse.h>
#include <Wire.h>

// --- Display Configuration ---
#define GFX_BL 23
Arduino_DataBus *bus =
    new Arduino_HWSPI(15 /* DC */, 14 /* CS */, 1 /* SCK */, 2 /* MOSI */);
Arduino_GFX *gfx = new Arduino_ST7789(
    bus, 22 /* RST */, 0 /* rotation */, false /* IPS */, 172 /* width */,
    320 /* height */, 34 /*col_offset1*/, 0 /*uint8_t row_offset1*/,
    34 /*col_offset2*/, 0 /*row_offset2*/);

void lcd_reg_init(void) {
  static const uint8_t init_operations[] = {
      BEGIN_WRITE,
      WRITE_COMMAND_8,
      0x11, // 2: Out of sleep mode, no args, w/delay
      END_WRITE,
      DELAY,
      120,

      BEGIN_WRITE,
      WRITE_C8_D16,
      0xDF,
      0x98,
      0x53,
      WRITE_C8_D8,
      0xB2,
      0x23,

      WRITE_COMMAND_8,
      0xB7,
      WRITE_BYTES,
      4,
      0x00,
      0x47,
      0x00,
      0x6F,

      WRITE_COMMAND_8,
      0xBB,
      WRITE_BYTES,
      6,
      0x1C,
      0x1A,
      0x55,
      0x73,
      0x63,
      0xF0,

      WRITE_C8_D16,
      0xC0,
      0x44,
      0xA4,
      WRITE_C8_D8,
      0xC1,
      0x16,

      WRITE_COMMAND_8,
      0xC3,
      WRITE_BYTES,
      8,
      0x7D,
      0x07,
      0x14,
      0x06,
      0xCF,
      0x71,
      0x72,
      0x77,

      WRITE_COMMAND_8,
      0xC4,
      WRITE_BYTES,
      12,
      0x00,
      0x00,
      0xA0,
      0x79,
      0x0B,
      0x0A,
      0x16,
      0x79,
      0x0B,
      0x0A,
      0x16,
      0x82,

      WRITE_COMMAND_8,
      0xC8,
      WRITE_BYTES,
      32,
      0x3F,
      0x32,
      0x29,
      0x29,
      0x27,
      0x2B,
      0x27,
      0x28,
      0x28,
      0x26,
      0x25,
      0x17,
      0x12,
      0x0D,
      0x04,
      0x00,
      0x3F,
      0x32,
      0x29,
      0x29,
      0x27,
      0x2B,
      0x27,
      0x28,
      0x28,
      0x26,
      0x25,
      0x17,
      0x12,
      0x0D,
      0x04,
      0x00,

      WRITE_COMMAND_8,
      0xD0,
      WRITE_BYTES,
      5,
      0x04,
      0x06,
      0x6B,
      0x0F,
      0x00,

      WRITE_C8_D16,
      0xD7,
      0x00,
      0x30,
      WRITE_C8_D8,
      0xE6,
      0x14,
      WRITE_C8_D8,
      0xDE,
      0x01,

      WRITE_COMMAND_8,
      0xB7,
      WRITE_BYTES,
      5,
      0x03,
      0x13,
      0xEF,
      0x35,
      0x35,

      WRITE_COMMAND_8,
      0xC1,
      WRITE_BYTES,
      3,
      0x14,
      0x15,
      0xC0,

      WRITE_C8_D16,
      0xC2,
      0x06,
      0x3A,
      WRITE_C8_D16,
      0xC4,
      0x72,
      0x12,
      WRITE_C8_D8,
      0xBE,
      0x00,
      WRITE_C8_D8,
      0xDE,
      0x02,

      WRITE_COMMAND_8,
      0xE5,
      WRITE_BYTES,
      3,
      0x00,
      0x02,
      0x00,

      WRITE_COMMAND_8,
      0xE5,
      WRITE_BYTES,
      3,
      0x01,
      0x02,
      0x00,

      WRITE_C8_D8,
      0xDE,
      0x00,
      WRITE_C8_D8,
      0x35,
      0x00,
      WRITE_C8_D8,
      0x3A,
      0x05,

      WRITE_COMMAND_8,
      0x2A,
      WRITE_BYTES,
      4,
      0x00,
      0x22,
      0x00,
      0xCD,

      WRITE_COMMAND_8,
      0x2B,
      WRITE_BYTES,
      4,
      0x00,
      0x00,
      0x01,
      0x3F,

      WRITE_C8_D8,
      0xDE,
      0x02,

      WRITE_COMMAND_8,
      0xE5,
      WRITE_BYTES,
      3,
      0x00,
      0x02,
      0x00,

      WRITE_C8_D8,
      0xDE,
      0x00,
      WRITE_C8_D8,
      0x36,
      0x00,
      WRITE_COMMAND_8,
      0x21,
      END_WRITE,

      DELAY,
      10,

      BEGIN_WRITE,
      WRITE_COMMAND_8,
      0x29, // 5: Main screen turn on, no args, w/delay
      END_WRITE};
  bus->batchOperation(init_operations, sizeof(init_operations));
}

// --- Touch Configuration ---
#define Touch_I2C_SDA 18
#define Touch_I2C_SCL 19
#define Touch_RST 20
#define Touch_INT 21

// --- BLE Mouse ---
BleMouse bleMouse("CyberScroll", "ESP32", 100);

// --- State Variables ---
bool wasConnected = false;
int lastTouchY = -1;
int scrollThreshold = 4;
touch_data_t touch_data;
unsigned long lastScrollTime = 0;
int visualOffset = 0; // For animating the screen

void drawCyberpunkUI(bool connected) {
  int w = gfx->width();
  int h = gfx->height();

  // Background
  gfx->fillScreen(RGB565_BLACK);

  // Top header line
  gfx->drawFastHLine(0, 10, w, RGB565_CYAN);
  gfx->drawFastHLine(0, 30, w, RGB565_CYAN);

  // Status Text
  gfx->setTextSize(2);
  gfx->setCursor(10, 14);
  if (connected) {
    gfx->setTextColor(RGB565_GREEN);
    gfx->print("SYS: ON");
  } else {
    gfx->setTextColor(RGB565_RED);
    gfx->print("SYS: OFF");
  }

  // Draw cyber grid/scroll area
  gfx->drawRect(10, 40, w - 20, h - 50, RGB565_DARKGREY);

  // Decorative lines
  gfx->drawLine(10, 40, 20, 30, RGB565_CYAN);
  gfx->drawLine(w - 10, 40, w - 20, 30, RGB565_CYAN);
  gfx->drawLine(10, h - 10, 20, h, RGB565_CYAN);
  gfx->drawLine(w - 10, h - 10, w - 20, h, RGB565_CYAN);

  gfx->setTextSize(1);
  gfx->setTextColor(RGB565_CYAN);
  gfx->setCursor(16, 50);
  gfx->print("> SCROLL");
  gfx->setCursor(16, 62);
  gfx->print("> DECK");
}

void updateScrollVisual(int deltaY) {
  int w = gfx->width();
  int h = gfx->height();
  int innerH = h - 52;

  // Clear inner area
  gfx->fillRect(11, 41, w - 22, innerH, RGB565_BLACK);

  // Update visual offset (inverted since scrolling up moves bars down)
  visualOffset += deltaY * 2;
  visualOffset = visualOffset % 20;
  if (visualOffset < 0)
    visualOffset += 20;

  // Draw moving neon bars
  for (int y = 41 + visualOffset; y < h - 12; y += 20) {
    if (y > 41) {
      gfx->fillRect(20, y, w - 40, 4, RGB565_MAGENTA);
    }
  }

  gfx->setTextColor(RGB565_CYAN);
  gfx->setCursor(16, 80);
  gfx->setTextSize(1);
  gfx->print("IN: ");
  gfx->print(deltaY);
}

void setup() {
  Serial.begin(115200);

  // Init BLE Mouse
  bleMouse.begin();

  // Init Display
  gfx->begin();
  lcd_reg_init();
  gfx->setRotation(2); // Portrait 180deg
  gfx->fillScreen(RGB565_BLACK);

  // Backlight active
  pinMode(GFX_BL, OUTPUT);
  digitalWrite(GFX_BL, HIGH);

  // Init Touch
  Wire.begin(Touch_I2C_SDA, Touch_I2C_SCL);
  bsp_touch_init(&Wire, Touch_RST, Touch_INT, gfx->getRotation(), gfx->width(),
                 gfx->height());

  // Initial draw
  drawCyberpunkUI(false);
}

void loop() {
  // Check connection status
  bool currentConnection = bleMouse.isConnected();
  if (currentConnection != wasConnected) {
    wasConnected = currentConnection;
    drawCyberpunkUI(wasConnected);
  }

  // Check touch
  bsp_touch_read();
  bool touchpad_pressed = bsp_touch_get_coordinates(&touch_data);

  if (touchpad_pressed) {
    int currentY = touch_data.coords[0].y;

    // Smooth scrolling calculation
    if (lastTouchY == -1) {
      lastTouchY = currentY;
    } else {
      int deltaY = currentY - lastTouchY;

      if (abs(deltaY) >= scrollThreshold && currentConnection) {
        // Send scroll event
        int scrollAmount = (deltaY < 0) ? -1 : 1;
        // Optionally map a faster drag to a faster scroll
        if (abs(deltaY) > 15) {
          scrollAmount *= 3;
        } else if (abs(deltaY) > 8) {
          scrollAmount *= 2;
        }

        bleMouse.move(0, 0, scrollAmount);
        lastTouchY = currentY;

        // Update screen
        if (millis() - lastScrollTime > 30) {
          updateScrollVisual(deltaY);
          lastScrollTime = millis();
        }
      } else if (!currentConnection) {
        // Just update visual if not connected
        if (abs(deltaY) >= scrollThreshold && millis() - lastScrollTime > 30) {
          updateScrollVisual(deltaY);
          lastScrollTime = millis();
          lastTouchY = currentY;
        }
      }
    }
  } else {
    lastTouchY = -1;
  }

  delay(10); // Standard loop delay
}
