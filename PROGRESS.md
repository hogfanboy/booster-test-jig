# PROGRESS

## Current target
Simple PCB V1 (90 x 70 mm, all through-hole): order 5 boards, then bring up the firmware on a real board.

## Working state (2026-11-23, after the designer's manual rework)
- Rebuilt in EasyEDA with controller library parts: U3 ESP32-C3 board, U4 TP4056 module (USB-C at the top edge),
  J1/J2/J8/J9 controller mounting holes, BT1-BT3 buttons (column on the right: OK, UP, DOWN), SG2 + Q2 buzzer,
  U1 HT7833, CN2 battery JST (back side), SW1 switch on the left edge.
- Stations: MT3608 footprint "1" (input on the right) and SX1308 footprint U2 (input on the left); inputs meet in the middle.
- Dividers: R2/R6 100k/10k (BOOST OUT, between the stations), R7/R8 10k/10k (battery).
- BTN_UP moved off GPIO6 (OLED SCL) to GPIO7 (U3 pin 13): schematic, PCB pad nets and trace. GPIO6 pin is now no-connect.
- Fully routed on top (1 mm power, 0.25 mm signal), GND pour on the bottom. DRC 0 errors.
- `hardware/netlist.txt` and `hardware/layout.json` are now OUT OF DATE: EasyEDA is the source of truth.

- Pogo holes: all 8 are 2.4 mm (fits the R125-4S sleeve). Spacings measured on the real modules by the designer:
  MT3608 30.48 x 7.62 mm, SX1308 17.78 x 10.16 mm. Station "1" renamed "MT3608". DRC 0 after pour rebuild (2026-11-23).
- Silkscreen: RED / BLACK (back) and + / - (top) at the battery connector CN2.

- HT7833 pin order (1 GND, 2 VIN, 3 VOUT) checked against the real part by the user. U2 name left as is (user's call).
- Final DRC before ordering (2026-11-23): pours rebuilt, 0 errors. Design is ready to order.

- Firmware written (2026-11-25): `firmware/BoosterTestJig/BoosterTestJig.ino`, how-to in `firmware/README.md`.
  Pins: BUZZER GPIO3, BTN_DOWN GPIO4, BTN_UP GPIO7, BTN_OK GPIO10, VOUT_SENSE GPIO0 (x11), VBAT_SENSE GPIO1 (x2),
  OLED SDA 5 / SCL 6. GitHub Action compiles it on every push. Not yet run on hardware.

## Next step
1. Order 5 boards (2 layers, 1.6 mm, HASL). Check the R125 sleeves fit the 2.4 mm holes.
2. When boards arrive: upload the firmware, check the OLED, buttons, buzzer, then calibrate against a multimeter.
