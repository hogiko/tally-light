#include "TallyLight.h"

#include <ESP_WiFiManager.h>

int toggle = 0;

const char* TALLY_LIGHT_ESSID    = "TallyLight";
const char* TALLY_LIGHT_PASSWORD = "47110815";

boolean saveSettingsDirty = false;

void connectSaveCallback() {
  saveSettingsDirty = true;
}

#ifndef XXX
void dumpFile(const char * path) {
  Serial.printf("Reading file: %s\n", path);

  File file = LittleFS.open(path, "r");
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.print("Read from file: ");
  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();

  Serial.println();
}
#endif

void TallyLight::connectInit() {

  tallyPixels.setPixels(BLACK, BLACK, BLACK, BLUE, BLUE, BLACK, BLACK, BLACK);
  delay(1000);

  dumpFile("/wifi.json");
  dumpFile("/tally-light.json");
/*  
 */

  boolean force = buttonPressed(3, 25, BLUE) || !strcmp(wifiSsid, "unknown");
  boolean reset = force && buttonPressed(6, 12, BLUE);
  boolean format = reset && buttonPressed(6, 12, ORANGE);

  WiFi.mode(WIFI_STA);                                                           
  WiFi.begin(wifiSsid, wifiPassword);                   
 
  Serial.printf("WiFi.begin(%s,%s)\n", wifiSsid, wifiPassword); 

  if (force) {
    connectSetup(force, reset, format);
  }

}

void TallyLight::connectSetup(boolean force, boolean reset, boolean format) {
  Serial.printf("connectSetup(force: %d, reset: %d, format: %d)\n", force, reset, format);

  WiFi.mode(WIFI_STA);

  ESP_WiFiManager wifiManager;

  wifiManager.setConfigPortalTimeout(120); 
  //set config save notify callback
  wifiManager.setSaveConfigCallback(connectSaveCallback);

  //set static ip
  //wifiManager.setSTAStaticIPConfig(IPAddress(10, 0, 1, 99), IPAddress(10, 0, 1, 1), IPAddress(255, 255, 255, 0));

  // define and add all your parameters here
  ESP_WMParameter parameterTallyControllerIp("tallyControllerIp", "Tally Controller IP", tallyControllerIp, 15);
  ESP_WMParameter parameterTallyLightName   ("tallyLightName"   , "Tally Light Name"   , tallyLightName   , 32);

  wifiManager.addParameter(&parameterTallyControllerIp);
  wifiManager.addParameter(&parameterTallyLightName);

  // run special actions on demand
  if (format) {
    tallyPixels.setPixels(ORANGE);
    formatSettings();
  }

  if (reset) {
    tallyPixels.setPixels(PURPLE);
    wifiManager.resetSettings();
    Serial.println("resetSettings done");
  }

  tallyPixels.setPixels(BLUE);

  bool wifiManagerState;

  Serial.println("startConfigPortal(TallyLight, ****)");
  wifiManagerState = wifiManager.startConfigPortal(TALLY_LIGHT_ESSID, TALLY_LIGHT_PASSWORD);
      
  if (!wifiManagerState) {
    Serial.println("failed to connect and hit timeout");
    tallyPixels.showError(BLUE, 15);
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(5000);
  }

  Serial.println("** connected");
  strcpy(wifiSsid, wifiManager.WiFi_SSID().c_str());
  strcpy(wifiPassword, wifiManager.WiFi_Pass().c_str());
  strcpy(tallyControllerIp, parameterTallyControllerIp.getValue());
  strcpy(tallyLightName, parameterTallyLightName.getValue());
  
  Serial.printf("** ssid=%s\n", wifiSsid);
  Serial.printf("** pass=%s\n", wifiPassword);
  Serial.printf("** ip  =%s\n", tallyControllerIp);
  Serial.printf("** name=%s\n", tallyLightName);

  if (saveSettingsDirty) {
    Serial.println("saveSettings");
    saveWifiSettings(wifiSsid, wifiPassword); 
    saveTallySettings(tallyControllerIp, tallyLightName); 
  }
}

void TallyLight::connectLoop() {
  Serial.printf("waiting for AP...%d \n", toggle);
  if (toggle++ & 1) {
    Serial.println("waiting odd");
    tallyPixels.setPixels(BLUE, BLACK, BLUE, BLACK, BLUE, BLACK, BLUE, BLACK);
  } else {
    tallyPixels.setPixels(BLACK, BLUE, BLACK, BLUE, BLACK, BLUE, BLACK, BLUE);
    Serial.println("waiting even");
  }
  delay(1000);

  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("AP connected %d\n", toggle);
    delay(1000);
    startPhase(RUN_PHASE);
  } else {
    delay(500);
  }

}
