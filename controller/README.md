# ESP8266 ESP-NOW Firmware

Ez a projekt kétféle firmware-t tartalmaz:
- `controller`: egy központi egység, amely ciklikusan kommunikál ESP-NOW-on keresztül legfeljebb 16 taggel.
- `tag`: egy ESP8266 eszköz, amely LED-et vezérel, szenzort kezel, és válaszol a controller parancsaira.

## Parancsformátum (8 bájt)
| Byte | Funkció |
|------|---------|
| 0    | Parancs (0x01 = RGB LED, 0x02 = Szenzor reset) |
| 1–3  | RGB LED értékek (R, G, B) vagy idő (resetnél) |
| 4–7  | Jövőbeli bővítésekhez, jelenleg 0x00 |

