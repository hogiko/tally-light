#include "TallyLight.h"


const char* tallyConfigFile = "/tally-light.json";
const char* wifiConfigFile = "/wifi.json";

//void fsTest();
//void deleteFile(const char * path);

void TallyLight::formatSettings() {

  for (int i = 0; (i < 10); i++) {
    tallyPixels.setPixels(RED);
    delay(500);
    tallyPixels.setPixels(BLACK);
    delay(500);    
  }

  tallyPixels.setPixels(RED);
  delay(2000);  
//  deleteFile("/wifi_cred.dat");
  fileSystem.format();
  tallyPixels.setPixels(GREEN);    
  delay(1000);    
}

void TallyLight::loadSettings() {
  Serial.printf("**** loadSettings()\n");
  
  if (!fileSystem.begin()) {
    Serial.println("fileSystem down");
    tallyPixels.showError(GRAY, 1);
    return;
  }

  JsonFileSettings tallySettings(tallyConfigFile);
  if (!tallySettings.read()) {
    Serial.printf("configFile not available %s\n", tallyConfigFile);
    tallyPixels.showError(GRAY, 2);
  } else {
    strcpy(tallyControllerIp, tallySettings.get("controllerIp"));
    strcpy(tallyLightName, tallySettings.get("name"));
    tallySettings.close();

    // patch ' ' to '+'
    for (char* tallyLightNameChar = tallyLightName; (*tallyLightNameChar); tallyLightNameChar++) {
      switch (*tallyLightNameChar) {
        case ' ':
          *tallyLightNameChar = '+';
          break;
      }
    }    
  }

  Serial.printf("****   ip   =%s\n", tallyControllerIp);
  Serial.printf("****   name =%s\n", tallyLightName);

  JsonFileSettings wifiSettings(wifiConfigFile); 
  if (!wifiSettings.read()) { 
    Serial.printf("configFile not available %s\n", wifiConfigFile); 
    tallyPixels.showError(GRAY, 3); 
  } else { 
    strcpy(wifiSsid, wifiSettings.get("ssid")); 
    strcpy(wifiPassword, wifiSettings.get("password")); 
    wifiSettings.close(); 
  } 
 
  Serial.printf("****   ssid='%s'\n", wifiSsid); 
  Serial.printf("****   pass='%s'\n", wifiPassword); 

}

void TallyLight::saveWifiSettings(const char* newWifiSsid, const char* newWifiPassword) { 
  Serial.printf("saveWifiSettings(%s,%s)...\n", newWifiSsid, newWifiPassword); 
 
  if (!fileSystem.begin()) { 
    tallyPixels.showError(GRAY, 1); 
    return; 
  }  
 
  strcpy(wifiSsid, newWifiSsid); 
  strcpy(wifiPassword, newWifiPassword); 
 
  JsonFileSettings wifiSettings(wifiConfigFile); 
  if (!wifiSettings.write() 
    .set("ssid", wifiSsid) 
    .set("password", wifiPassword) 
    .flush()) { 
    Serial.printf("could not write %s\n", wifiConfigFile); 
    tallyPixels.showError(GRAY, 4); 
  } else { 
    wifiSettings.close(); 
  } 
} 
 
void TallyLight::saveTallySettings(const char* newTallyControllerIp, const char* newTallyLightName) { 
  Serial.printf("saveTallySettings(%s,%s)...\n", newTallyControllerIp, newTallyLightName);

  if (!fileSystem.begin()) {
    tallyPixels.showError(GRAY, 1);
    return;
  } 

  strcpy(tallyControllerIp, newTallyControllerIp);
  strcpy(tallyLightName, newTallyLightName);

  JsonFileSettings tallySettings(tallyConfigFile);
  if (!tallySettings.write()
    .set("controllerIp", tallyControllerIp)
    .set("name", tallyLightName)
    .flush()) {
    Serial.printf("could not write %s\n", tallyConfigFile);
    tallyPixels.showError(GRAY, 5);
  } else {
    tallySettings.close();
  }
}

#ifdef XXX
void TallyLight::saveTallySettings(const char* newTallyControllerIp, const char* newTallyLightName) {
  Serial.println("saveSettings()...");

  strcpy(tallyControllerIp, newTallyControllerIp);
  strcpy(tallyLightName, newTallyLightName);

  if (!fileSystem.begin()) {
    tallyPixels.showError(GRAY, 1);
    return;
  } 

  File file = fileSystem.open(tallyConfigFile, "w");
  if (!file) {
    tallyPixels.showError(GRAY, 4);
    return;
  }

  StaticJsonDocument<256> doc;
  doc["controllerIp"] = tallyControllerIp;
  doc["name"] = tallyLightName;
  if (!serializeJson(doc, file)) {
    tallyPixels.showError(GRAY, 5);
    Serial.println(F("Failed to write to file"));
    return;
  }

  file.close();

  Serial.println("The values to be stored are: ");
  Serial.println("**** ip   =" + String(tallyControllerIp));
  Serial.println("**** name =" + String(tallyLightName));

  delay(3000);
}

#include <time.h>

void listDir(const char * dirname) {
  Serial.printf("Listing directory: %s\n", dirname);

  Dir root = LittleFS.openDir(dirname);

  while (root.next()) {
    File file = root.openFile("r");
    Serial.print("  FILE: ");
    Serial.print(root.fileName());
    Serial.print("  SIZE: ");
    Serial.print(file.size());
    time_t cr = file.getCreationTime();
    time_t lw = file.getLastWrite();
    file.close();
    struct tm * tmstruct = localtime(&cr);
    Serial.printf("    CREATION: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
    tmstruct = localtime(&lw);
    Serial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
  }
}


void readFile(const char * path) {
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
}

void writeFile(const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);

  File file = LittleFS.open(path, "w");
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  delay(2000); // Make sure the CREATE and LASTWRITE times are different
  file.close();
}

void appendFile(const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = LittleFS.open(path, "a");
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if (file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

void renameFile(const char * path1, const char * path2) {
  Serial.printf("Renaming file %s to %s\n", path1, path2);
  if (LittleFS.rename(path1, path2)) {
    Serial.println("File renamed");
  } else {
    Serial.println("Rename failed");
  }
}

void deleteFile(const char * path) {
  Serial.printf("Deleting file: %s\n", path);
  if (LittleFS.remove(path)) {
    Serial.println("File deleted");
  } else {
    Serial.println("Delete failed");
  }
}

void fsTest() {
  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed");
    return;
  }
  listDir("/");
  deleteFile("/hello.txt");
  writeFile("/hello.txt", "Hello ");
  appendFile("/hello.txt", "World!\n");
  //writeFile("/tally-light.json", "{\"controllerIp\": \"10.47.11.109\"\n,\"name\":\"Kamera 1\"\n}\n");

  listDir("/");

  Serial.println("The timestamp should be valid above");

  Serial.println("Now unmount and remount and perform the same operation.");
  Serial.println("Timestamp should be valid, data should be good.");
  LittleFS.end();
  Serial.println("Now mount it");
  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed");
    return;
  }
  readFile("/hello.txt");
  listDir("/");
}

#endif
