#ifndef NeoPixels_h
#define NeoPixels_h

#include <NeoPixelBus.h>

class NeoPixels {
  private:
    int pin;
    int size;
    NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *pixels;

    RgbColor ORANGE;
    RgbColor RED;
    RgbColor BLACK;

  public:
    NeoPixels(int pin, int size);
    ~NeoPixels();

    NeoPixels& start();
    NeoPixels& show();

    NeoPixels& setPixel(uint16_t index, RgbColor& color);
    NeoPixels& setPixels(RgbColor& color);
    NeoPixels& setPixels(RgbColor& c0, RgbColor& c1, RgbColor& c2, RgbColor& c3, RgbColor& c4, RgbColor& c5, RgbColor& c6, RgbColor& c7);
    NeoPixels& pushPixel(RgbColor& color);
    NeoPixels& popPixel(RgbColor& color);

    NeoPixels& showError(RgbColor& color, uint8_t error);  
    NeoPixels& showError(RgbColor& color, uint8_t error, int count, int sleep);  
    NeoPixels& flashError(RgbColor& color, int sleep);  
    
};

#endif
