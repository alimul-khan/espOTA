#include <Arduino.h>
#include "wifi_manager.h"
#include "web_server.h"
#include "ota_handler.h"

void setup() {
  Serial.begin(115200);
  delay(200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // off (active low)

  wifiManagerSetup();  // STA with saved creds, else AP fallback
  webServerSetup();    // status + config pages
  otaSetup();          // OTA ready (works in STA; can also work while in AP)
}

void loop() {
  wifiManagerLoop();
  webServerLoop();
  otaLoop();

  // tiny heartbeat
  static uint32_t t=0; static bool s=false;
  if (millis()-t>100) { t=millis(); s=!s; digitalWrite(LED_BUILTIN, s?LOW:HIGH); }
}
