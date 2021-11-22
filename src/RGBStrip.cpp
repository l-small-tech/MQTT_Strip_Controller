#ifndef RGBSTRIP
#define RGBSTRIP
#endif

/*
    White High density Strip: BTF-LIGHTING RGBW RGBNW Natural White SK6812 (Similar WS2812B)
    Black Lower Density Strip: SK6812 RGBW RGB Warm RGBNW Led Strip

    On the Wemos D1 Mini, Use D7 as the Data Pin. D7 == GPIO 13 == MOSI
    Use 430-470 Ohm with the data pin
*/

#include <FastLED.h>
#include <Arduino.h>

// ---- Parameters ----
#define DATA_PIN    13
#define NUM_LEDS    50
#define BRIGHTNESS  100
#define FPS         2
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB


class RGBStrip {
    
private:

  CRGB* leds = new CRGB[NUM_LEDS];
  CRGB* leds_bkp = new CRGB[NUM_LEDS];
  int brightness = BRIGHTNESS;

  short mode;

  unsigned long thisTime;
  unsigned long lastTime = 0;


public:

    RGBStrip() {
        FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
        FastLED.setBrightness(brightness);

        // Initialize strip to a nice sky blue :)
        setStripColor(0, 0, 0);
    }

    void effect_RunningLights(byte red, byte green, byte blue, int WaveDelay) {
        int Position=0;
 
        for(int j=0; j<NUM_LEDS*2; j++) {
                Position++; // = 0; //Position + Rate;
                for(int i=0; i<NUM_LEDS; i++) {
                // sine wave, 3 offset waves make a rainbow!
                //float level = sin(i+Position) * 127 + 128;
                //setPixel(i,level,0,0);
                //float level = sin(i+Position) * 127 + 128;
                setPixel(i,((sin(i+Position) * 127 + 128)/255)*red,
                            ((sin(i+Position) * 127 + 128)/255)*green,
                            ((sin(i+Position) * 127 + 128)/255)*blue);
                }
                
                showStrip();
                delay(WaveDelay);
        }
    }

    void loopHook() {
        effect_RunningLights(50, 100, 150, 50);
    }

    void on() {
        // Set all LEDs to the state they had before beign turned off        
        for (int i = 0; i < NUM_LEDS; i++) {
            leds[i] = leds_bkp[i];
        }

        FastLED.show();
    }

    void off() {
        // Save the state of the strip
        for (int i = 0; i < NUM_LEDS; i++) {
            leds_bkp[i] = leds[i];
        }

        // Turn all the lights off
        for (int i = 0; i < NUM_LEDS; i++) {
            leds[i] = CRGB::Black;
        }

        FastLED.show();
    }

    void setPixelColor(uint16_t i, uint8_t r, uint8_t g, uint8_t b) {
        leds[i] = CRGB(r, g, b);

        FastLED.show();

        // Serial.printf("Set pixel %d color to R: %d, G: %d, B: %d\n", i, r, g, b);
    }

    void push(int r, int g, int b) {
        // Set first led to color
        leds_bkp[0] = CRGB(r, g, b);

        // Shift all the leds one place down the strip
        for (int i = 0; i < NUM_LEDS - 1; i++) {
            leds_bkp[i + 1] = leds[i];
        }

        // Copy the new shifted data to the strip
        for (int i = 0; i < NUM_LEDS; i++) {
            leds[i] = leds_bkp[i];
        }

        FastLED.show();
    }

    void setAll(byte red, byte green, byte blue) {
        for(int i = 0; i < NUM_LEDS; i++ ) {
            setPixel(i, red, green, blue);
        }
        showStrip();
    }
    
    void setBrightness(int b) {

        this->brightness = b;
        FastLED.setBrightness(b);

        FastLED.show();
    }

    void setPixel(int Pixel, byte red, byte green, byte blue) {
        leds[Pixel].r = red;
        leds[Pixel].g = green;
        leds[Pixel].b = blue;
    }

    void setStripColor(uint8_t r, uint8_t g, uint8_t b) {

        for (int i = 0; i < NUM_LEDS; i++) {
            leds[i] = CRGB(r, g, b);
        }

        FastLED.show();

        Serial.printf("Set strip color to R: %d, G: %d, B: %d\n", r, g, b);
    }

    inline void showStrip() { FastLED.show(); }
};