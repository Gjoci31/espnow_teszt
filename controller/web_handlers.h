#include "eeprom_config.h"
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <espnow.h>

extern AsyncWebServer server;

void parseMac(const String& str, uint8_t* mac) {
  sscanf(str.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
         &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
}

void setupWebHandlers() {
  server.on("/cmd", HTTP_GET, [](AsyncWebServerRequest *request){
    uint8_t mac[6];
    uint8_t data[8] = {0x01, 0, 0, 0, 0, 0, 0, 0};

    if (request->hasParam("mac") && request->hasParam("rgb")) {
      parseMac(request->getParam("mac")->value(), mac);
      String rgb = request->getParam("rgb")->value();
      if (rgb.length() == 6) {
        data[1] = strtol(rgb.substring(0, 2).c_str(), NULL, 16);
        data[2] = strtol(rgb.substring(2, 4).c_str(), NULL, 16);
        data[3] = strtol(rgb.substring(4, 6).c_str(), NULL, 16);
      }
      esp_now_send(mac, data, sizeof(data));
      request->send(200, "text/plain", "Parancs elküldve");
    } else {
      request->send(400, "text/plain", "Hibás paraméter");
    }
  });

  server.on("/read_sensor", HTTP_GET, [](AsyncWebServerRequest *request){
    // Szenzorkérés küldése
    if (request->hasParam("mac")) {
      uint8_t mac[6];
      uint8_t data[8] = {0x03, 0, 0, 0, 0, 0, 0, 0};
      parseMac(request->getParam("mac")->value(), mac);
      esp_now_send(mac, data, sizeof(data));
      request->send(200, "text/plain", "Szenzorkérés elküldve");
    } else {
      request->send(400, "text/plain", "MAC hiányzik");
    }
  });
}
