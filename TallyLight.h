#ifndef Included_TallyLight_h
#define Included_TallyLight_h

#include <Arduino.h>

#include "NeoPixels.h"
#include <NeoPixelBus.h>

#include <LittleFS.h>
#include "FS.h"
#define fileSystem LittleFS

#include <ArduinoJson.h>

#define INIT_PHASE    0
#define CONNECT_PHASE 1
#define RUN_PHASE     2

#define PIXEL_COUNT 2*8
#define PIXEL_PIN   2

#define BUTTON_PIN 0

class JsonFileSettings {
  private:
    const char* fileName;
    File        file;

    StaticJsonDocument<512> doc;

  public:
     JsonFileSettings(const char* fileName)
       : fileName(fileName) {
     }

     boolean read() {
       file = fileSystem.open(fileName, "r");
       DeserializationError error = deserializeJson(doc, file);
       return !error;
     }

     JsonFileSettings& write() {
       file = fileSystem.open(fileName, "w");
       return *this;
     }

     const char* get(const char* name) {
      return doc[name];
     }

     JsonFileSettings& set(const char* name, const char* value) {
       doc[name] = value;
       return *this;
     }

     boolean flush() {
       return serializeJson(doc, file);
     }
 
     void close() {
       file.close();
     }

     operator bool() const {
       return file;
     }
};

class TallyLight {

  private:
    int phase_ = -1;

    RgbColor RED;
    RgbColor GREEN;
    RgbColor BLUE;
    RgbColor YELLOW;
    RgbColor ORANGE;
    RgbColor PURPLE;
    RgbColor BLACK;
    RgbColor WHITE;
    RgbColor GRAY;
    RgbColor LIME;

    NeoPixels tallyPixels;

    char      tallyControllerIp[16]  = "1.2.3.4";
    char      tallyLightName[33]     = "none";
    
    int       tallyControllerPort    = 8080;
    int       tallyControllerTimeout = 15000;

    char      wifiSsid[64]           = "unknown";
    char      wifiPassword[64]       = "unknown";
    
  public:
    TallyLight(int COLOR_SATURATION)
      : tallyPixels(PIXEL_COUNT, PIXEL_PIN),
        RED(COLOR_SATURATION, 0, 0),
        GREEN(0, COLOR_SATURATION, 0),
        BLUE(0, 0, COLOR_SATURATION),
        YELLOW(COLOR_SATURATION, COLOR_SATURATION, 0),
        ORANGE(COLOR_SATURATION, COLOR_SATURATION / 2, 0),
        PURPLE(COLOR_SATURATION / 2, 0, COLOR_SATURATION),
        BLACK(0),
        GRAY(COLOR_SATURATION / 2, COLOR_SATURATION / 2, COLOR_SATURATION / 2),
        WHITE(COLOR_SATURATION, COLOR_SATURATION, COLOR_SATURATION),
        LIME(3 * COLOR_SATURATION / 4, COLOR_SATURATION, 0) {
    }

    void startPhase(int phase) {
      Serial.printf("*** phase: %d -> %d\n", phase_, phase);
      phase_ = phase;

      switch (phase) {
        case INIT_PHASE:
          Serial.println("*** initInit() ***");
          initInit();
          Serial.println("*** /initInit() ***");
          break;

        case CONNECT_PHASE:
          Serial.println("*** connectInit() ***");
          connectInit();
          Serial.println("*** /connectInit() ***");
          break;

        case RUN_PHASE:
          Serial.println("*** runInit() ***");
          runInit();
          Serial.println("*** /runInit() ***");
          break;
      }
    }

    void loop() {
      switch (phase_) {
        case INIT_PHASE:
          initLoop();
          break;

        case CONNECT_PHASE:
          connectLoop();
          break;

        case RUN_PHASE:
          runLoop();
          break;

        default:
          Serial.printf("*** unknown phase(%d)! ***\n", phase_);
          delay(1000);
      }
    }

    void initInit();
    void initLoop();

    void connectInit();
    void connectSetup(boolean force, boolean reset, boolean format);
    void connectLoop();

    void runInit();
    void runLoop();

    void formatSettings();
    void loadSettings();
    void saveWifiSettings(const char* wifiSsid, const char* wifiPassword);
    void saveTallySettings(const char* controllerHostname, const char* tallyLightName);

    boolean buttonPressed() {
      boolean result = digitalRead(BUTTON_PIN) == LOW;
      while (result && digitalRead(BUTTON_PIN) == LOW) {
        delay(1);
      }
      return result;
    }

    boolean buttonPressed(int loop, int sleep, RgbColor& color) {
      tallyPixels.setPixels(BLACK);

      while (loop-- > 0) {
        for (int i = 0; (i < PIXEL_COUNT); i++) {
          tallyPixels.pushPixel(color);
          delay(sleep);
          if (buttonPressed()) {
            tallyPixels.setPixels(color);
            return true;
          }
        }
        for (int i = 0; (i < PIXEL_COUNT); i++) {
          tallyPixels.popPixel(BLACK);
          delay(sleep);
          if (buttonPressed()) {
            tallyPixels.setPixels(color);
            return true;
          }
        }
      }

      return false;
    }

};

#endif
