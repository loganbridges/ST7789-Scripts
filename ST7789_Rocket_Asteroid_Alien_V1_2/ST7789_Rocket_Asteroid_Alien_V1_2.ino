#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>

#define TFT_CS      14
#define TFT_RST     17
#define TFT_DC      15
#define TFT_MOSI    23
#define TFT_SCLK    18
#define TFT_BLACK   13

// Use hardware SPI
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// With 1024 stars the update rate is ~65 frames per second
#define NSTARS 1024
uint8_t sx[NSTARS] = {};
uint8_t sy[NSTARS] = {};
uint8_t sz[NSTARS] = {};

uint8_t za, zb, zc, zx;

// Rocket position
int rocketX = 160; // Initial horizontal position (centered)
int rocketY = 220; // Fixed vertical position (near bottom)
int rocketDirection = 1; // 1 = moving right, -1 = moving left
int rocketSpeed = 3; // Speed of horizontal movement

// Flame animation variables
bool flameGrow = true;
int flameHeight = 12; // Initial flame height (bigger flames)

// Asteroid variables
bool asteroidActive = false;
int asteroidX = 0;
int asteroidY = 0;
int asteroidSpeed = 5;
int asteroidSize = 6;
uint16_t asteroidColor = 0;

// UFO variables
bool ufoActive = false;
int ufoX = 0;
int ufoY = 40; // Fixed vertical position
int ufoSpeed = 8;
int ufoSpawnCounter = 0; // Counter for rare spawning

// Fast 0-255 random number generator
uint8_t __attribute__((always_inline)) rng() {
  zx++;
  za = (za ^ zc ^ zx);
  zb = (zb + za);
  zc = ((zc + (zb >> 1)) ^ za);
  return zc;
}

// Function to draw the rocket
void drawRocket() {
  int bodyWidth = 16; // Larger body width
  int bodyHeight = 32; // Larger body height

  // Erase previous rocket and flames
  tft.fillRect(rocketX - 20, rocketY - 48, 40, 70, TFT_BLACK); // Adjusted to include flame area

  // Rocket Body
  tft.fillRect(rocketX - bodyWidth / 2, rocketY - bodyHeight, bodyWidth, bodyHeight, tft.color565(255, 255, 255));

  // Rocket Nose
  tft.fillTriangle(rocketX - bodyWidth / 2, rocketY - bodyHeight, 
                   rocketX + bodyWidth / 2, rocketY - bodyHeight, 
                   rocketX, rocketY - bodyHeight - 16, tft.color565(255, 0, 0)); // Larger nose

  // Flames
  tft.fillTriangle(rocketX - 6, rocketY, 
                   rocketX + 6, rocketY, 
                   rocketX, rocketY + flameHeight, tft.color565(255, 165, 0)); // Larger flame
}

// Function to update the asteroid
void updateAsteroid() {
  if (!asteroidActive) {
    // Randomly spawn an asteroid
    if (rng() < 10) { // Adjust spawn chance
      asteroidActive = true;
      asteroidX = random(320);
      asteroidY = 0;
      asteroidSize = random(3, 9); // Varying size
      int grayShade = random(100, 200); // Random gray shade
      asteroidColor = tft.color565(grayShade, grayShade, grayShade); // Set gray color
    }
  } else {
    // Erase the old asteroid
    tft.fillCircle(asteroidX, asteroidY, asteroidSize, TFT_BLACK);

    // Move asteroid down
    asteroidY += asteroidSpeed;

    // Check if it goes off screen
    if (asteroidY > 240) {
      asteroidActive = false; // Reset asteroid
    } else {
      // Draw the updated asteroid
      tft.fillCircle(asteroidX, asteroidY, asteroidSize, asteroidColor);
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
  za = random(256);
  zb = random(256);
  zc = random(256);
  zx = random(256);

  Serial.begin(115200);
  tft.init(240, 320, SPI_MODE3);
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
}

void loop() {
  unsigned long t0 = micros();
  uint8_t spawnDepthVariation = 255;

  // Update stars
  for (int i = 0; i < NSTARS; ++i) {
    if (sz[i] <= 1) {
      sx[i] = 160 - 120 + rng();
      sy[i] = rng();
      sz[i] = spawnDepthVariation--;
    } else {
      int old_screen_x = ((int)sx[i] - 160) * 256 / sz[i] + 160;
      int old_screen_y = ((int)sy[i] - 120) * 256 / sz[i] + 120;

      // Erase old star
      tft.drawPixel(old_screen_x, old_screen_y, TFT_BLACK);

      sz[i] -= 2;
      if (sz[i] > 1) {
        int screen_x = ((int)sx[i] - 160) * 256 / sz[i] + 160;
        int screen_y = ((int)sy[i] - 120) * 256 / sz[i] + 120;

        if (screen_x >= 0 && screen_y >= 0 && screen_x < 320 && screen_y < 240) {
          uint8_t r, g, b;
          r = g = b = 255 - sz[i];
          tft.drawPixel(screen_x, screen_y, tft.color565(r, g, b));
        } else
          sz[i] = 0; // Out of screen, reset.
      }
    }
  }

  // Update rocket position
  rocketX += rocketDirection * rocketSpeed;

  // Change direction at edges
  if (rocketX < 20 || rocketX > 300) {
    rocketDirection *= -1; // Reverse direction
  }

  // Update flame animation
  if (flameGrow) {
    flameHeight++;
    if (flameHeight >= 18) flameGrow = false; // Bigger flame range
  } else {
    flameHeight--;
    if (flameHeight <= 12) flameGrow = true; // Reset flame range
  }

  // Update and draw elements
  drawRocket();
  updateAsteroid();
  updateUFO();

  unsigned long t1 = micros();
  Serial.println(1.0 / ((t1 - t0) / 1000000.0)); // Calculate and print frames per second

  delay(30); // Slight delay to control animation speed
}
