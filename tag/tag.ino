#include <ESP8266WiFi.h>
#include <espnow.h>
#include "eeprom_config.h"
#include "config_mode.h"

#define RGB_R_PIN D1
#define RGB_G_PIN D2
#define RGB_B_PIN D3
#define SENSOR_PWR_PIN D5
#define CONFIG_MODE_PIN D6
#define SENSOR_IN_PIN A0

uint8_t controllerMac[6];

void setup() {
  Serial.begin(115200);
  pinMode(CONFIG_MODE_PIN, INPUT);
  pinMode(SENSOR_PWR_PIN, OUTPUT);
  digitalWrite(SENSOR_PWR_PIN, HIGH);

  loadConfig();
  memcpy(controllerMac, deviceConfig.ctrlMac, 6);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  wifi_set_channel(deviceConfig.wifiChannel);
  esp_now_init();
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(onDataRecv);
  esp_now_add_peer(controllerMac, ESP_NOW_ROLE_CONTROLLER, deviceConfig.wifiChannel, NULL, 0);

  analogWriteFreq(500);

  if (digitalRead(CONFIG_MODE_PIN) == HIGH) {
    char ssid[20];
    sprintf(ssid, "SMBS-%02X%02X%02X", deviceConfig.selfMac[3], deviceConfig.selfMac[4], deviceConfig.selfMac[5]);
    startConfigPortal(ssid);
  }
}

void loop() {}

void onDataRecv(uint8_t* mac, uint8_t* data, uint8_t len) {
  if (len < 8) return;
  uint8_t cmd = data[0];

  if (cmd == 0x01) {
    analogWrite(RGB_R_PIN, data[1]);
    analogWrite(RGB_G_PIN, data[2]);
    analogWrite(RGB_B_PIN, data[3]);
  } else if (cmd == 0x02) {
    int seconds = data[1];
    digitalWrite(SENSOR_PWR_PIN, LOW);
    delay(seconds * 1000);
    digitalWrite(SENSOR_PWR_PIN, HIGH);
  } else if (cmd == 0x03) {
    uint16_t sensorValue = analogRead(SENSOR_IN_PIN);
    uint8_t response[8] = {0x83, (uint8_t)(sensorValue >> 8), (uint8_t)(sensorValue & 0xFF), 0, 0, 0, 0, 0};
    esp_now_send(controllerMac, response, sizeof(response));
  }
}
