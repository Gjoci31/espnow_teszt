#pragma once
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include "eeprom_config.h"

AsyncWebServer server(80);

void parseMac(const String& str, uint8_t* mac) {
  sscanf(str.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
         &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
}

void startConfigPortal(const char* ssid) {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, "Sensopower");

  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS hiba");
    return;
  }

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.on("/save", HTTP_GET, [](AsyncWebServerRequest *request){
    String response = "Kapott paraméterek:<br>";

    if (request->hasParam("self")) {
      parseMac(request->getParam("self")->value(), deviceConfig.selfMac);
      response += "Saját MAC beállítva<br>";
    }
    if (request->hasParam("ctrl")) {
      parseMac(request->getParam("ctrl")->value(), deviceConfig.ctrlMac);
      response += "Controller MAC beállítva<br>";
    }
    if (request->hasParam("ch")) {
      deviceConfig.wifiChannel = request->getParam("ch")->value().toInt();
      response += "Csatorna beállítva<br>";
    }

    saveConfig();
    response += "<br><strong>Beállítások elmentve EEPROM-ba.</strong>";
    request->send(200, "text/html", response);
  });

  server.begin();
  Serial.println("Webes konfiguráció elérhető");
}
