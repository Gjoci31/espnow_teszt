#pragma once
#include <EEPROM.h>

struct DeviceConfig {
  uint8_t selfMac[6];
  uint8_t ctrlMac[6];
  uint8_t wifiChannel;
};

DeviceConfig deviceConfig;

void loadConfig() {
  EEPROM.begin(sizeof(DeviceConfig));
  EEPROM.get(0, deviceConfig);
  EEPROM.end();
}

void saveConfig() {
  EEPROM.begin(sizeof(DeviceConfig));
  EEPROM.put(0, deviceConfig);
  EEPROM.commit();
  EEPROM.end();
}
