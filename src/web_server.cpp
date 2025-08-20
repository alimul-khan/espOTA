#include "web_server.h"
#include "wifi_manager.h"
#include <ESP8266WebServer.h>

ESP8266WebServer server(80);

String makeStatusPage() {
  String html = "<!DOCTYPE html><html><head><title>ESP Status</title>";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; background:#f5f5f5; color:#333; text-align:center; padding:40px; }";
  html += "h2 { color:#0066cc; margin-bottom:20px; }";
  html += "p { font-size:16px; margin:10px 0; }";
  html += "a { display:inline-block; margin-top:20px; padding:10px 20px; background:#0066cc; color:#fff; text-decoration:none; border-radius:6px; }";
  html += "a:hover { background:#004a99; }";
  html += "</style></head><body>";
  html += "<h2>ESP8266 Status</h2>";
  html += "<p><b>Connected SSID:</b> " + wifiManagerGetSSID() + "</p>";
  html += "<p><b>IP Address:</b> " + wifiManagerGetIP() + "</p>";
  html += "<a href='/config'>Configure WiFi</a>";
  html += "</body></html>";
  return html;
}

String makeConfigPage() {
  String html = "<!DOCTYPE html><html><head><title>ESP WiFi Config</title>";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; background:#f5f5f5; color:#333; text-align:center; padding:40px; }";
  html += "h2 { color:#0066cc; margin-bottom:20px; }";
  html += "form { display:inline-block; background:#fff; padding:20px 30px; border-radius:8px; box-shadow:0 2px 6px rgba(0,0,0,0.1); }";
  html += "input[type=text], input[type=password] { width:100%; padding:10px; margin:8px 0; border:1px solid #ccc; border-radius:4px; }";
  html += "input[type=submit] { background:#0066cc; color:white; border:none; padding:10px 20px; border-radius:6px; cursor:pointer; }";
  html += "input[type=submit]:hover { background:#004a99; }";
  html += "</style></head><body>";
  html += "<h2>Configure WiFi</h2>";
  html += "<form action='/save' method='POST'>";
  html += "<label>SSID:</label><br><input type='text' name='ssid'><br>";
  html += "<label>Password:</label><br><input type='password' name='pass'><br>";
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
