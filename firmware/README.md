# Firmware: BoosterTestJig

One Arduino sketch: `BoosterTestJig/BoosterTestJig.ino`.

## Using the jig
| Screen / button | What it means |
|---|---|
| Top left `T5.00` | Your target voltage |
| Top right `3.9V` | Battery. Blinks `LOW` below 3.4 V: charge it |
| Big number | The module's output voltage right now |
| Bottom `▲ UP 0.25` | Output is 0.25 V too low: turn the pot to raise it |
| Bottom `▼ DN 0.25` | Output is 0.25 V too high: turn the pot to lower it |
| `ON TARGET` + beep | Within 0.05 V of the target |
| `NO MODULE` | Nothing on the pogo pins (output below 0.5 V) |
| UP / DOWN | Change the target by 0.1 V. Hold to repeat; after 2 s it moves 0.5 V per step |
| OK (short press) | Save the target, so it comes back after power off (one beep) |

The display reads up to about 27 V; above that it shows `OVER 27V`.

## Calibrating (do this once per board)
1. Put a module on the pins and set its output to something like 12 V. Hold a multimeter on the output pads.
2. Hold **OK** for 2 seconds. The screen says `CAL` (long beep).
3. Press **UP** / **DOWN** until the big number matches the multimeter (0.01 V per press).
4. Short press **OK** to save (two beeps). A low buzz means it was more than 20 % off and nothing was saved.
5. To undo a bad calibration: in `CAL`, hold **OK** for 2 seconds again (low beep, back to factor 1.000).

## Uploading (Arduino IDE)
1. **Board support:** File > Preferences > "Additional boards manager URLs", add
   `https://espressif.github.io/arduino-esp32/package_esp32_index.json`. Then Tools > Board > Boards Manager,
   search "esp32", install **esp32 by Espressif Systems**.
2. **Library:** Tools > Manage Libraries, search **U8g2**, install it (by oliver).
3. Open `BoosterTestJig.ino`. Tools > Board > esp32 > **MakerGO ESP32 C3 SuperMini**.
   Tools > USB CDC On Boot > **Enabled** (so the Serial Monitor works).
4. Plug in the ESP32 by its own USB-C, pick its port under Tools > Port, press Upload.
   If upload doesn't start: hold the BOOT button, tap RESET, let go of BOOT, then Upload again.

## Uploading (Arduino CLI, Windows Git Bash)
```
CLI=/c/Users/hogfa/AppData/Local/Programs/arduino-cli/arduino-cli.exe
$CLI lib install U8g2
$CLI compile --fqbn esp32:esp32:makergo_c3_supermini firmware/BoosterTestJig
$CLI upload  --fqbn esp32:esp32:makergo_c3_supermini -p COM5 firmware/BoosterTestJig   # your COM port
```

## Serial Monitor (115200 baud)
Prints `vout`, `raw` (before calibration), `vbat`, `target` and `cal` twice a second. Handy for checking the dividers.
