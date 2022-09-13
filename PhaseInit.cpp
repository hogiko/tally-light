#include "TallyLight.h"

void TallyLight::initInit() {
  // this resets all the pixels to an off state
  tallyPixels.start();

  loadSettings();

  startPhase(CONNECT_PHASE);
}

void TallyLight::initLoop() {
  Serial.println("initLoop()");
  delay(1000);
}
