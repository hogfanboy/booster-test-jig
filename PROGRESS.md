# PROGRESS

## Current target
Simple PCB V1 (90 x 70 mm, all through-hole): fix the open items below, then order 5 boards.

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

## Open items
1. Silkscreen: give U2 a name (e.g. SX1308); optional +/- marks at the pogo pads.
2. Check the HT7833 pin order (footprint: 1 GND, 2 VIN, 3 VOUT) against the part you have.
3. Then order. Firmware: BUZZER = GPIO3, BTN_DOWN = GPIO4, BTN_UP = GPIO7, BTN_OK = GPIO10,
   VOUT_SENSE = GPIO0 (x11), VBAT_SENSE = GPIO1 (x2).
