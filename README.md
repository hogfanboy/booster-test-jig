# BoosterTestJig

A bench tool for setting the trimmer pot on MT3608 and SX1308 boost-converter modules.
Drop a module onto the spring-loaded test pins, turn its pot, and read the output voltage on the
ESP32's built-in OLED. Three buttons set a target voltage.

- ESP32-C3 SuperMini with built-in 0.42" OLED (plugs into female headers)
- 1S LiPo + TP4056 charger module, slide switch, HT7833 3.3 V regulator
- Two pogo-pin stations (R125-4S sleeves + P125-E2 probes): MT3608 and SX1308
- All through-hole, 90 x 70 mm, four corner holes for a 3D-printed case

Spec and verified details: [PROJECT-NOTES.md](PROJECT-NOTES.md). Status: [PROGRESS.md](PROGRESS.md).
