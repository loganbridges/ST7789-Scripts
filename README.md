# ST7789-Scripts

// This repo has scripts for the ADAfruit-ST7789 and esp32-WROOM-32 (The Board Module/Manager used is ESP32-WROOM-DA)

// The libraries most commonly needed are =

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>

// The pin layout always needed is =

#define TFT_CS      14
#define TFT_RST     17
#define TFT_DC      15
#define TFT_BLACK   13

// The hardware SPI that is always needed is =

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// The neeed libraries to download include AdafruitGFX, Adafruit_st7789_xxx
