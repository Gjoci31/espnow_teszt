#include <ESP8266WiFi.h>
#include <espnow.h>
#include "eeprom_config.h"
#include "config_mode.h"
#include "web_handlers.h"
#include "serial_handler.h"
#include "utils.h"

#define CONFIG_MODE_PIN D3
#define NUM_TAGS 16
#define MESSAGE_INTERVAL 100

uint8_t tagMacs[NUM_TAGS][6] = {
  {0x24, 0x6F, 0x28, 0xAA, 0xBB, 0x01},
  {0x24, 0x6F, 0x28, 0xAA, 0xBB, 0x02},
};

unsigned long lastSendTime = 0;
int currentTag = 0;

void setup() {
  Serial.begin(115200);
  pinMode(CONFIG_MODE_PIN, INPUT);

  loadConfig();
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  wifi_set_channel(deviceConfig.wifiChannel);
  esp_now_init();
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);

  for (int i = 0; i < NUM_TAGS; i++) {
    esp_now_add_peer(tagMacs[i], ESP_NOW_ROLE_SLAVE, deviceConfig.wifiChannel, NULL, 0);
  }

  checkIncomingESP();

  if (digitalRead(CONFIG_MODE_PIN) == HIGH) {
    startConfigPortal("SMBS-CTRL");
    setupWebHandlers();
  }
}

void loop() {
  handleSerialCommand();

  unsigned long now = millis();
  if (now - lastSendTime >= MESSAGE_INTERVAL) {
    sendToTag(currentTag);
    currentTag = (currentTag + 1) % NUM_TAGS;
    lastSendTime = now;
  }
}

void sendToTag(int index) {
  uint8_t data[8] = {0x01, 0xFF, 0x00, 0x00, 0, 0, 0, 0};
  esp_now_send(tagMacs[index], data, sizeof(data));
}

void onDataRecv(uint8_t* mac, uint8_t* data, uint8_t len) {
  Serial.print("ESP-NOW adat érkezett ");
  printMac(mac);
  Serial.print(": ");
  for (uint8_t i = 0; i < len; i++) {
    if (data[i] < 0x10) Serial.print("0");
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}

void checkIncomingESP() {
  esp_now_register_recv_cb(onDataRecv);
}
