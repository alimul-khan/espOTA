#include "web_server.h"
#include "wifi_manager.h"
#include <ESP8266WebServer.h>

ESP8266WebServer server(80);

String makeStatusPage() {
  String html = "<!DOCTYPE html><html><head><title>ESP Status</title></head><body>";
  html += "<h2>ESP8266 Status</h2>";
  html += "<p><b>Connected SSID:</b> " + wifiManagerGetSSID() + "</p>";
  html += "<p><b>IP Address:</b> " + wifiManagerGetIP() + "</p>";
  html += "<p><a href='/config'>Configure WiFi</a></p>";
  html += "</body></html>";
  return html;
}

String makeConfigPage() {
  String html = "<!DOCTYPE html><html><head><title>ESP WiFi Config</title></head><body>";
  html += "<h2>Configure WiFi</h2>";
  html += "<form action='/save' method='POST'>";
  html += "SSID: <input type='text' name='ssid'><br>";
  html += "Password: <input type='password' name='pass'><br><br>";
  html += "<input type='submit' value='Save'>";
  html += "</form>";
  html += "</body></html>";
  return html;
}

void webServerSetup() {
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", makeStatusPage());
  });

  server.on("/config", HTTP_GET, []() {
    server.send(200, "text/html", makeConfigPage());
  });

  server.on("/save", HTTP_POST, []() {
    if (server.hasArg("ssid") && server.hasArg("pass")) {
      String newSSID = server.arg("ssid");
      String newPASS = server.arg("pass");
      Serial.printf("[Web] Saving creds: SSID=%s\n", newSSID.c_str());

      // ✅ call the public function from wifi_manager.h
      wifiManagerSaveCreds(newSSID.c_str(), newPASS.c_str());

      server.send(200, "text/html", "<h2>Saved! Rebooting...</h2>");
      delay(1000);
      ESP.restart();
    } else {
      server.send(400, "text/html", "Missing SSID or Password!");
    }
  });

  server.begin();
  Serial.println("[Web] Server started on port 80");
}

void webServerLoop() {
  server.handleClient();
}
