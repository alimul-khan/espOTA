#include "ota_handler.h"
#include <ArduinoOTA.h>

void otaSetup() {
  ArduinoOTA.onStart([]() { Serial.println("\n[OTA] Start"); });
  ArduinoOTA.onEnd([]() { Serial.println("\n[OTA] End"); });
  ArduinoOTA.onProgress([](unsigned int p, unsigned int t) {
    Serial.printf("[OTA] %u%%\r", (p * 100) / t);
  });
  ArduinoOTA.onError([](ota_error_t err) {
    Serial.printf("[OTA] Error %u\n", err);
  });
  ArduinoOTA.setHostname("espnode");
  ArduinoOTA.begin();
  Serial.println("[OTA] Ready");
}

void otaLoop() {
  ArduinoOTA.handle();
}
