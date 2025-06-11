#include "crc8.h"

#define FRAME_MAX 32
uint8_t frameBuf[FRAME_MAX];
uint8_t frameLen = 0;
bool inFrame = false;

void handleSerialCommand() {
  while (Serial.available()) {
    uint8_t b = Serial.read();

    if (!inFrame) {
      if (b == 0x45) {
        frameBuf[0] = b;
        while (Serial.available() < 1);  // várjunk még egy bájtra
        b = Serial.read();
        if (b == 0x54) {
          frameBuf[1] = b;
          while (Serial.available() < 1);
          frameBuf[2] = Serial.read();  // hossz
          frameLen = frameBuf[2];
          if (frameLen <= FRAME_MAX) {
            for (int i = 3; i < frameLen; i++) {
              while (Serial.available() < 1);
              frameBuf[i] = Serial.read();
            }
            uint8_t crc = frameBuf[frameLen - 1];
            uint8_t calc = crc8(frameBuf, frameLen - 1);
            if (crc == calc) {
              processCommand(frameBuf + 3, frameLen - 4);
            } else {
              Serial.println("CRC hiba!");
            }
          }
        }
      }
    }
  }
}

void processCommand(uint8_t* cmdData, uint8_t len) {
  uint8_t cmd = cmdData[0];
  else {
    Serial.print("Ismeretlen parancs: 0x");
    Serial.println(cmd, HEX);
  }
}
