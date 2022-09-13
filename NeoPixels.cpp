#include "NeoPixels.h"


NeoPixels::NeoPixels(int size, int pin)
  : ORANGE(128, 64,0),
    RED(128, 0, 0),
    BLACK(0) {
  this->size = size;
  this->pin = pin;

  pixels = new NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>(size, pin);
}

NeoPixels::~NeoPixels() {
}

NeoPixels& NeoPixels::start() {
  pixels->Begin();
  pixels->Show();
  return *this;
}

NeoPixels& NeoPixels::show() {
  pixels->Show();
  return *this;
}

NeoPixels& NeoPixels::setPixel(uint16_t index, RgbColor& color) {
  pixels->SetPixelColor(index, color);
  return *this;
}

NeoPixels& NeoPixels::setPixels(RgbColor& color) {
  for (int i = 0; (i < size); i++) {
    pixels->SetPixelColor(i, color);
  }
  show();
  return *this;
}

NeoPixels& NeoPixels::setPixels(RgbColor& c0, RgbColor& c1, RgbColor& c2, RgbColor& c3, RgbColor& c4, RgbColor& c5, RgbColor& c6, RgbColor& c7) {
   for (int i = 0; (i < size); i += 8) {
      setPixel(i + 0, c0);
      setPixel(i + 1, c1);
      setPixel(i + 2, c2);
      setPixel(i + 3, c3);
      setPixel(i + 4, c4);
      setPixel(i + 5, c5);
      setPixel(i + 6, c6);
      setPixel(i + 7, c7);
   }
   show();
   return *this;
}

NeoPixels& NeoPixels::pushPixel(RgbColor& color) {
  for (int i = size-1; (i > 0); ) {
    pixels->SetPixelColor(i, pixels->GetPixelColor(--i));
  }
  pixels->SetPixelColor(0, color);
  show();
  return *this;
}

NeoPixels& NeoPixels::popPixel(RgbColor& color) {
  for (int i = 0; (i < size); ) {
    pixels->SetPixelColor(i, pixels->GetPixelColor(++i));
  }
  pixels->SetPixelColor(size-1, color);
  show();
  return *this;
}

NeoPixels& NeoPixels::showError(RgbColor& color, uint8_t error) {
  return showError(color, error, 1, 250);
}

NeoPixels& NeoPixels::showError(RgbColor& color, uint8_t error, int count, int sleep) {
  while (count-- > 0) {
    setPixels(
      ORANGE, ORANGE, 
      color, color,
      (error & 0x8) ? RED : BLACK,
      (error & 0x4) ? RED : BLACK,
      (error & 0x2) ? RED : BLACK,
      (error & 0x1) ? RED : BLACK
    );
    delay(sleep);
    setPixels(BLACK);
    delay(sleep);
  }
  
  return *this;
}

NeoPixels& NeoPixels::flashError(RgbColor& color, int sleep) {
  RgbColor* keep = new RgbColor(size);
  for (int i = 0; (i < size); i++) {
    keep[i] = pixels->GetPixelColor(i);
    setPixel(i, color);
  }
  show();
  delay(sleep);
  for (int i = 0; (i < size); i++) {
    setPixel(i, keep[i]);
  }
  show();
  delete keep;
  return *this;
}
