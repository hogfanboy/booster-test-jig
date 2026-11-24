# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

BoosterTestJig: a simple all-through-hole jig for setting the trimmer pot on MT3608 and SX1308 boost modules.
ESP32-C3 SuperMini (0.42" OLED) on headers, 1S LiPo + TP4056 module, HT7833, pogo-pin stations (R125-4S + P125-E2),
3 buttons. One user, 3-5 boards: keep it simple and reuse parts the user has (see ESP NOW Controller). PCB designed in EasyEDA Pro through its API. Firmware not written yet.

**Read [PROGRESS.md](PROGRESS.md) first.** [PROJECT-NOTES.md](PROJECT-NOTES.md) is the spec and settled truth (parts, pins, power, firmware requirements); read only the part you need.

## Docs (four files, one job each)
- `CLAUDE.md`: permanent rules and commands only. No brainstorming.
- `PROGRESS.md`: current target, what just broke, next step. If an idea changes direction, replace the old target.
- `PROJECT-NOTES.md`: the spec (acts as the FSD) and settled truth. Add only things that are built and verified; no history or failed attempts.
- `IDEAS.md`: backlog and parking lot for untested ideas. Don't read it unless asked or when picking the next task. When the user says "park this", append one line under "Backlog / Parking Lot" and go back to the current task without discussing it. At session end, if asked, promote verified items to PROJECT-NOTES.md and remove them.
- `tools/easyeda/API-NOTES.md`: EasyEDA Pro API gotchas. **Read it before writing any EasyEDA script.**

## Rules
- The user is a beginner with Arduino code and toolchain settings: explain in plain-language steps.
- Through-hole parts only. Keep it simple; avoid parts the user would have to order from AliExpress. Board under 100 x 100 mm.
- `hardware/netlist.txt` is the source of truth for connections; `hardware/layout.json` for positions (mm).
  Module pad positions in `layout.json` are estimates until measured with calipers.
- Always run DRC after any PCB change, and trust DRC over the autorouter's report.
- Never commit `media/`. Commit as hogfanboy (noreply), never the Gmail.
- After any design change, feature or bug fix, update PROGRESS.md (under 60 lines): working state, active bug or next task, key decisions (no failed attempts). Then say: "Task complete and PROGRESS.md updated. Ready for /clear."

## Layout
- `hardware/`: netlist.txt, layout.json (board, parts, pogo stations, holes).
- `tools/easyeda/`: `call.py` (talks straight to the bridge on 127.0.0.1:49620-49629) and `scripts/` (reusable board scripts).
- `firmware/`: Arduino sketch (none yet). `docs/`: future GitHub Pages site / web flasher.

## EasyEDA bridge
1. User: EasyEDA Pro open with the BoosterTestJig project; Run API Gateway extension has "Allow interact with external" on.
2. API server: `cd %USERPROFILE%\Documents\EasyEDA-Claude\easyeda-api-skill-main` then `npm run server`.
3. User: EasyEDA menu API Gateway > Reconnect.
4. From `tools/easyeda`: `python call.py health` (also `windows`, `exec '<js>'`, `execfile <file.js> [timeout_s]`). No relay needed.

IDs: project `19dcc0327b9440189d38008c62ef5c1c`; schematic page `1bd8bd7bf6a09b1d`; PCB `eb04a63bfb02ce56`.
Start every PCB script with `await eda.dmt_EditorControl.openDocument("eb04a63bfb02ce56")` plus a short wait.

## Build (firmware, once it exists)
Arduino CLI: `/c/Users/hogfa/AppData/Local/Programs/arduino-cli/arduino-cli.exe` (not on PATH).
Board: MakerGO ESP32 C3 SuperMini (`esp32:esp32:makergo_c3_supermini`), same as the ESP-NOW car (to confirm on this board).
