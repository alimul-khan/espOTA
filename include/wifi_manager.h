#pragma once
#include <Arduino.h>

#define WIFI_SSID_MAX 32
#define WIFI_PASS_MAX 64

// Boot/connect: try saved STA creds; on failure start AP "ESP-Setup"/"12345678"
void wifiManagerSetup();

// Call in loop (keeps state, optional reconnects)
void wifiManagerLoop();

// Save new creds to EEPROM (will apply after reboot)
void wifiManagerSaveCreds(const char* ssid, const char* pass);

// Clear creds (next boot will start AP)
void wifiManagerClearCreds();

// Status helpers
String wifiManagerGetSSID();      // current SSID (or "ESP-Setup" in AP)
String wifiManagerGetIP();        // STA IP or AP IP
bool   wifiManagerIsConnected();  // true if STA connected
