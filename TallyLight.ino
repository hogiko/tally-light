#include "TallyLight.h"

TallyLight tallyLight(64);

void setup() {

  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  while (!Serial) {
    delay(1);
  }
  Serial.setDebugOutput(true);

  Serial.println("setup");

  delay(3000);

  tallyLight.startPhase(INIT_PHASE);
}

void loop() {
  tallyLight.loop();
}
