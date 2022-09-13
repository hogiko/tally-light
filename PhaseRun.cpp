#include "TallyLight.h"


#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#include <WiFiClient.h>

#include <ESP8266HTTPClient.h>

const String TALLY_LIGHT_API = "/api/controller/tallyLight";

String tallyControllerUrl;
String tallyControllerUrlTimeout;

boolean runWithTimeout = false;

class Statistics {
  private:
    char* name;
    int count;
    long tookMs;
    
  public:
    Statistics(char* name) {
      this->name = name;
    }

    void feed(long tookMs) {
      this->count++;
      this->tookMs += tookMs;
    }

    void dump() {
      if (count) {
        Serial.printf("%s\t#%d\t~%ld\t%ld\n", name, count, tookMs/count, tookMs);
      }
    }
} allStats("all"), onAirStats("onAir"), standByStats("standBy"), blackStats("black"), failStats("fail");

void TallyLight::runInit() {
  Serial.println("initRun");

  loadSettings();

  tallyControllerTimeout  = 15000;

  tallyControllerUrl        = TALLY_LIGHT_API + "?name=" + tallyLightName;
  tallyControllerUrlTimeout = tallyControllerUrl + String("&timeout=") + tallyControllerTimeout;
}

void TallyLight::runLoop() {

  String tallyControllerNow = runWithTimeout ? tallyControllerUrlTimeout : tallyControllerUrl;
  
  WiFiClient client;
  HTTPClient http;

  http.setTimeout(4000 + (runWithTimeout ? tallyControllerTimeout : 0));

  long start = millis();
  if (http.begin(client, tallyControllerIp, tallyControllerPort, tallyControllerNow.c_str())) {
    int httpCode = http.GET();

    long finish = millis();
    allStats.feed(finish - start);

    // httpCode will be negative on error
    switch (httpCode) {
      case HTTP_CODE_OK: {
          String payload = http.getString();
          Serial.println(payload);

          DynamicJsonDocument doc(2048);

          char buffer[200];
          payload.toCharArray(buffer, 200);
          DeserializationError error = deserializeJson(doc, buffer);

          if (!error) {
            const char* tallyName  = doc["name"];
            const char* tallyState = doc["state"];
            const bool  onAir      = doc["onAir"];
            const bool  standBy    = doc["standBy"];

            if (onAir) {
              tallyPixels.setPixels(RED, RED, RED, RED, RED, RED, RED, RED);
              onAirStats.feed(finish - start);
          } else if (standBy) {
              tallyPixels.setPixels(GREEN, GREEN, GREEN, BLACK, BLACK, GREEN, GREEN, GREEN);
              standByStats.feed(finish - start);
            } else {
              tallyPixels.setPixels(BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK);
              blackStats.feed(finish - start);
            }

            runWithTimeout = true;
            
          } else {
            failStats.feed(finish - start);
            Serial.println("json parse error");
            tallyPixels.showError(GREEN, 1);
            runWithTimeout = false;
          }
          break;
        }

      default: {
          failStats.feed(finish - start);
          Serial.printf("[HTTP] GET... code: %d\n", httpCode);
          Serial.println(http.getString());
          Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
          if (httpCode > 0) {
            tallyPixels.showError(GREEN, (httpCode / 100) % 10);
            tallyPixels.showError(GREEN, (httpCode /  10) % 10);
            tallyPixels.showError(GREEN, (httpCode      ) % 10);
          } else {
            tallyPixels.setPixel(0, ORANGE);
            tallyPixels.setPixel(7, ORANGE);
            tallyPixels.show();
//          tallyPixels.flashError(ORANGE, 100);
//          tallyPixels.showError(LIME, (-httpCode / 100) % 10);
//          tallyPixels.showError(LIME, (-httpCode /  10) % 10);
//          tallyPixels.showError(LIME, (-httpCode      ) % 10);
          }
          delay(1000);
          runWithTimeout = false;
          break;
        }
    }

    http.end();
  }

  if (buttonPressed()) {
    Serial.printf("ip  ='%s'\n", tallyControllerIp);
    Serial.printf("port=%d\n", tallyControllerPort);
    Serial.printf("url ='%s'\n", tallyControllerNow.c_str());

    allStats.dump();
    onAirStats.dump();
    standByStats.dump();
    failStats.dump();
    
  }

}
