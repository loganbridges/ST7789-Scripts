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

// Function to draw the cockpit overlay with symmetrical angled lines
void drawCockpit() {
  uint16_t cockpitColor = tft.color565(80, 80, 80); // Slightly darker gray

  // Thicker lines using a loop
  for (int i = 0; i < 6; i++) { // Line thickness
    tft.drawLine(20 + i, 0, 120 + i, 180, cockpitColor);  // Left angled line
    tft.drawLine(200 - i, 180, 300 - i, 0, cockpitColor); // Right angled line (mirrored to match left side)
    tft.drawLine(120, 180 + i, 200, 180 + i, cockpitColor); // Thicker middle horizontal line
    tft.drawLine(120 + i, 180, 60 + i, 240, cockpitColor); // Left extended line to bottom
    tft.drawLine(200 - i, 180, 260 - i, 240, cockpitColor); // Right extended line to bottom
    tft.drawLine(60 + i, 240, 260 - i, 240, cockpitColor);  // Bottom connecting line
  }

  // Thin border around the screen
  tft.drawRect(0, 0, 320, 240, tft.color565(60, 60, 60)); // Thin darker border
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
  drawCockpit(); // Redraw cockpit every frame to ensure it remains visible
  delay(30);
}