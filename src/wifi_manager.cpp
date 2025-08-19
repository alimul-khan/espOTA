#include "wifi_manager.h"
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>

static const char* AP_SSID = "ESP-Setup";
static const char* AP_PASS = "12345678";

// EEPROM layout
struct Creds {
  uint32_t magic;                 // validity marker
  char ssid[WIFI_SSID_MAX];
  char pass[WIFI_PASS_MAX];
};
static const uint32_t CREDS_MAGIC = 0xC0DEC0DE;
static const size_t   EEPROM_SIZE = sizeof(Creds);

static Creds creds{};
static bool  wifiConnected = false;
static uint32_t lastRetryMs = 0;

static void eepromLoad() {
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.get(0, creds);
  EEPROM.end();
  // guard strings
  creds.ssid[WIFI_SSID_MAX-1] = 0;
  creds.pass[WIFI_PASS_MAX-1] = 0;
  if (creds.magic != CREDS_MAGIC) {
    memset(&creds, 0, sizeof(creds));
  }
}

static void eepromStore(const Creds& c) {
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.put(0, c);
  EEPROM.commit();
  EEPROM.end();
}

void wifiManagerSaveCreds(const char* s, const char* p) {
  Creds c{};
  c.magic = CREDS_MAGIC;
  strncpy(c.ssid, s ? s : "", WIFI_SSID_MAX-1);
  strncpy(c.pass, p ? p : "", WIFI_PASS_MAX-1);
  eepromStore(c);
  Serial.printf("[WiFi] Saved creds: SSID=\"%s\" (len %u)\n", c.ssid, (unsigned)strlen(c.ssid));
}

void wifiManagerClearCreds() {
  Creds c{};
  eepromStore(c);  // magic=0 clears
  Serial.println("[WiFi] Cleared creds");
}

static bool tryConnectSTA(uint32_t timeoutMs = 15000) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(creds.ssid, creds.pass);
  Serial.printf("[WiFi] Connecting to \"%s\"", creds.ssid);
  uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - start) < timeoutMs) {
    delay(250);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("\n[WiFi] Connected, IP: %s\n", WiFi.localIP().toString().c_str());
    
    WiFi.hostname("espnode");
    if (MDNS.begin("espnode")) {
    MDNS.addService("http", "tcp", 80);
    Serial.println("[mDNS] espnode.local (http)");
    } else {
    Serial.println("[mDNS] start failed");
    }

    wifiConnected = true;
    return true;
  }
  Serial.println("\n[WiFi] Connect failed");
  wifiConnected = false;
  return false;
}

static void startAP() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASS);
  IPAddress ip = WiFi.softAPIP();
  Serial.printf("[WiFi] AP mode: SSID=%s PASS=%s IP=%s\n",
                AP_SSID, AP_PASS, ip.toString().c_str());
  wifiConnected = false;
}

void wifiManagerSetup() {
  Serial.println("[WiFi] Loading saved credentials...");
  eepromLoad();

  if (creds.magic == CREDS_MAGIC && creds.ssid[0]) {
    if (!tryConnectSTA()) {
      Serial.println("[WiFi] Falling back to AP...");
      startAP();
    }
  } else {
    Serial.println("[WiFi] No saved creds, starting AP...");
    startAP();
  }
}

void wifiManagerLoop() {
  // Optional lightweight reconnect: if we *had* creds and lost Wi-Fi, retry every ~10s
  if (creds.magic == CREDS_MAGIC && creds.ssid[0]) {
    if (WiFi.getMode() == WIFI_STA && WiFi.status() != WL_CONNECTED) {
      if (millis() - lastRetryMs > 10000) {
        lastRetryMs = millis();
        Serial.println("[WiFi] Disconnected, retrying STA connect...");
        tryConnectSTA(5000);
      }
    }
  }
}

String wifiManagerGetSSID() {
  if (WiFi.getMode() == WIFI_AP) return String(AP_SSID);
  String s = WiFi.SSID();
  if (s.length() == 0 && creds.ssid[0]) s = creds.ssid; // fallback
  return s;
}

String wifiManagerGetIP() {
  if (WiFi.getMode() == WIFI_AP) return WiFi.softAPIP().toString();
  return WiFi.localIP().toString();
}

bool wifiManagerIsConnected() {
  return (WiFi.getMode() == WIFI_STA) && (WiFi.status() == WL_CONNECTED) && wifiConnected;
}
