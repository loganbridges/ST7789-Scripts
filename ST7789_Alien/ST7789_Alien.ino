#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>

#define TFT_CS      14
#define TFT_RST     17
#define TFT_DC      15
#define TFT_BLACK   13

// Use hardware SPI
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// With 1024 stars, update rate ~65 frames per second
#define NSTARS 1024
uint8_t sx[NSTARS] = {};
uint8_t sy[NSTARS] = {};
uint8_t sz[NSTARS] = {};

// RNG state variables
uint8_t za, zb, zc, zx;

// UFO variables
bool ufoActive = false;
int ufoX = 0;
int ufoY = 40; // Fixed vertical position
int ufoSpeed = 7;
int ufoSpawnCounter = 0; // Counter for rare spawning

// Random number generator
uint8_t __attribute__((always_inline)) rng() {
  zx++;
  za ^= zc ^ zx;
  zb += za;
  zc = ((zc + (zb >> 1)) ^ za);
  return zc;
}

// Function to draw stars
void drawStars() {
  uint8_t spawnDepthVariation = 255;

  for (int i = 0; i < NSTARS; ++i) {
    if (sz[i] <= 1) {
      sx[i] = 160 - 120 + rng();
      sy[i] = rng();
      sz[i] = spawnDepthVariation--;
    } else {
      int old_screen_x = ((int)sx[i] - 160) * 256 / sz[i] + 160;
      int old_screen_y = ((int)sy[i] - 120) * 256 / sz[i] + 120;

      tft.drawPixel(old_screen_x, old_screen_y, TFT_BLACK);

      sz[i] -= 2;
      if (sz[i] > 1) {
        int screen_x = ((int)sx[i] - 160) * 256 / sz[i] + 160;
        int screen_y = ((int)sy[i] - 120) * 256 / sz[i] + 120;

        if (screen_x >= 0 && screen_y >= 0 && screen_x < 320 && screen_y < 240) {
          uint8_t r, g, b;
          r = g = b = 255 - sz[i];
          tft.drawPixel(screen_x, screen_y, tft.color565(r, g, b));
        } else {
          sz[i] = 0;
        }
      }
    }
  }
}

// Function to draw a UFO with silver body and transparent dome
void drawUFO() {
  // Silver body
  tft.fillRect(ufoX - 10, ufoY, 20, 6, tft.color565(192, 192, 192)); // Silver rectangle

  // Stick figure inside dome
  tft.drawLine(ufoX, ufoY - 5, ufoX, ufoY - 2, tft.color565(0, 255, 0)); // Body
  tft.drawLine(ufoX, ufoY - 3, ufoX - 2, ufoY, tft.color565(0, 255, 0)); // Left leg
  tft.drawLine(ufoX, ufoY - 3, ufoX + 2, ufoY, tft.color565(0, 255, 0)); // Right leg
  tft.drawLine(ufoX, ufoY - 5, ufoX - 2, ufoY - 6, tft.color565(0, 255, 0)); // Left arm
  tft.drawLine(ufoX, ufoY - 5, ufoX + 2, ufoY - 6, tft.color565(0, 255, 0)); // Right arm
  tft.fillCircle(ufoX, ufoY - 7, 2, tft.color565(0, 255, 0)); // Head

  // Transparent dome
  tft.drawCircle(ufoX, ufoY - 3, 10, tft.color565(150, 150, 255)); // Light blue outline
}

// Function to update the UFO
void updateUFO() {
  if (!ufoActive) {
    // Rarely spawn a UFO
    if (ufoSpawnCounter++ > 500 && rng() < 5) { // Adjust spawn chance
      ufoActive = true;
      ufoX = 0; // Start from the left
      ufoSpawnCounter = 0;
    }
  } else {
    // Erase the old UFO
    tft.fillRect(ufoX - 18, ufoY - 18, 36, 36, TFT_BLACK);

    // Move UFO to the right
    ufoX += ufoSpeed;

    // Check if it goes off screen
    if (ufoX > 320) {
      ufoActive = false; // Reset UFO
    } else {
      // Draw the updated UFO
      drawUFO();
    }
  }
}

void setup() {
  // Initialize RNG state
  za = random(256);
  zb = random(256);
  zc = random(256);
  zx = random(256);

  tft.init(240, 320, SPI_MODE3);
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
}

void loop() {
  drawStars();
  updateUFO();
  delay(30);
}
