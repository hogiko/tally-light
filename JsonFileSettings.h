#ifndef JsonFileSettings_h
#define JsonFileSettings_h

#include <LittleFS.h>
#include "FS.h"
#define fileSystem LittleFS

#include <ArduinoJson.h>

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

#endif
