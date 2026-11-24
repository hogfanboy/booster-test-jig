# PROJECT-NOTES (spec / FSD)

Settled truth only. Items marked **[unverified]** are designed but not yet built or measured.
Built for one user, 3-5 boards. Keep it simple: through-hole, parts already on hand where possible.

## 1. Purpose
Set the output-voltage pot on MT3608 and SX1308 boost modules: the module sits on pogo pins, gets battery
power, and the ESP32's built-in 0.42" OLED shows its output voltage live while you turn the pot.

## 2. Hardware (V1) [unverified]
| Ref | Part (LCSC) | Job |
|---|---|---|
| J1, J2 | PM254-1-08-Z-8.5 female 1x8 (C2897371), rows 17.78 mm apart | ESP32-C3 SuperMini 0.42" OLED board |
| J3-J6 | PZ254V-11-01P single pins (C492400) | TP4056 module pads OUT+, B+, B-, OUT-. Same pin positions as the ESP NOW Controller footprint; module plugs in from the back, USB-C at the left edge |
| J7 | B2B-PH-K-S JST-PH 2 pin | 1S LiPo (pin 1 = +) |
| SW1 | SS12F23G5 slide (C719121), same as the ESP NOW Controller | Power |
| U2 | HT7333-A TO-92 footprint (C73726); fit an HT7833 | 3.3 V into the ESP32 3V3 pin |
| R1, R2 | 100k / 10k | Module output divider (27 V -> 2.45 V) |
| R3, R4 | 100k / 100k | Battery divider |
| SW2-SW4 | 12 x 12 x 7.3 mm tactile (C10861) | UP, OK, DOWN in a column on the right edge |
| SG1, Q1 | HY12-1P 12 mm passive buzzer (C17701083) + 2N7000 (C900717) | Beeps; same circuit as the controller (buzzer from VBAT_SW to drain) |
| Pogo | 8 pads (2 stations x 4), 2.4 mm plated hole, 3.8 mm pad | R125-4S sleeve (2.36 mm OD) + P125-E2 probe |

Board 90 x 70 mm, 2 layers. Four corner holes like the controller: 3.2 mm plated, 6 mm GND pad, 6 stitching vias, 3.5 mm in from each edge.

Layout (user sits at the bottom edge): pogo stations at the front (hands), ESP32 + OLED at the back edge with USB at the top
(never under a hand), buttons in a column on the right edge, switch / battery / TP4056 USB-C on the left edge.
Both stations face the same way (input left, output right, + on the far side) so polarity is the same on both.

## 3. Power
Battery -> J7 -> TP4056 B+/B-. No LDO capacitors (the controller runs without them).
 TP4056 OUT+ -> SW1 -> `VBAT_SW`. TP4056 OUT- = GND.
`VBAT_SW` feeds the LDO, the battery divider and the module input (VIN+ pogo). Module VIN- and VOUT- = GND.
A shorted module is stopped by the TP4056 module's protection chip. Charge through the TP4056 USB-C. ESP32 5V pin unused.

**HT7333 vs HT7833 pin order:** the footprint is the HT7333-A (pin 1 GND, 2 VIN, 3 VOUT). Check the HT7833's
pin order against its datasheet or your controller board before soldering; turn it around if it differs.

## 4. Pins (ESP32-C3)
| GPIO | Net | Use |
|---|---|---|
| 0 | VOUT_SENSE | Module output, 100k / 10k divider (x11) |
| 1 | VBAT_SENSE | Battery, 100k / 100k divider (x2) |
| 10 | BTN_UP | Button to GND, internal pull-up |
| 4 | BTN_DOWN | Button to GND, internal pull-up |
| 3 | BTN_OK | Button to GND, internal pull-up |
| 7 | BUZZER | 2N7000 gate (J2 pin 4: check against the board printing) |
| 5 / 6 | SDA / SCL | On-board 0.42" OLED |

## 5. Pogo stations (pad positions are ESTIMATES until measured)
Module sits pot-side up on the probes. Exact positions: `hardware/layout.json` (`stations`).

## 6. Firmware requirements (not written)
- Big live VOUT on the OLED; battery voltage in a corner.
- UP/DOWN set a target voltage, OK saves it; screen shows how far off you are (and an arrow: turn up/down).
- Calibrate the divider once against a multimeter and store the factor.
