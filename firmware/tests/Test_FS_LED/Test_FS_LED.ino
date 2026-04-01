/* Test firmware for LED
  PWNSAT Project 2025
*/
#include <Adafruit_NeoPixel.h>

#define PIN D0
#define NUMPIXELS 2

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_RGB + NEO_KHZ800);

void setup() { pixels.begin(); }

void loop() {
  pixels.clear();
  pixels.setPixelColor(0, pixels.Color(0, 150, 0));
  pixels.setPixelColor(1, pixels.Color(150, 150, 0));
  pixels.show();
  delay(500);
}
