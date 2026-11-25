// BoosterTestJig firmware
// ESP32-C3 SuperMini with built-in 0.42" OLED (72 x 40 pixels).
//
// What it does:
//   - Shows the boost module's output voltage in big digits.
//   - UP / DOWN set a target voltage, OK saves it (it is remembered after power off).
//   - The bottom line says how far off you are and which way to turn the pot.
//   - Beeps once when the output lands on the target.
//   - Hold OK for 2 seconds to calibrate against a multimeter.
//
// Board: MakerGO ESP32 C3 SuperMini (esp32:esp32:makergo_c3_supermini)
// Library: U8g2 (install it from the Arduino Library Manager)

#include <Arduino.h>
#include <U8g2lib.h>
#include <Preferences.h>
#include "Button.h"   // button debounce and hold-to-repeat

// ---------- Pins (from the PCB, PROGRESS.md) ----------
const int PIN_VOUT_SENSE = 0;   // module output through 100k / 10k divider
const int PIN_VBAT_SENSE = 1;   // battery through 10k / 10k divider
const int PIN_BUZZER     = 3;   // 2N7000 gate, passive buzzer
const int PIN_BTN_DOWN   = 4;   // buttons pull to GND
const int PIN_BTN_UP     = 7;
const int PIN_BTN_OK     = 10;
const int PIN_OLED_SDA   = 5;   // on-board OLED
const int PIN_OLED_SCL   = 6;

// ---------- Divider ratios ----------
const float VOUT_RATIO = (100.0f + 10.0f) / 10.0f;  // x11
const float VBAT_RATIO = (10.0f + 10.0f) / 10.0f;   // x2

// ---------- Settings ----------
const float TARGET_MIN   = 1.0f;    // V
const float TARGET_MAX   = 27.0f;   // V (ADC tops out near 2.5 V x 11)
const float TOLERANCE    = 0.05f;   // V: "on target" window
const float NO_MODULE_V  = 0.5f;    // below this: no module on the pins
const float LOW_BATT_V   = 3.4f;    // battery warning
const int   ADC_MAX_MV   = 2800;    // ADC reading above this is out of range

U8G2_SSD1306_72X40_ER_F_HW_I2C oled(U8G2_R0, U8X8_PIN_NONE, PIN_OLED_SCL, PIN_OLED_SDA);
Preferences prefs;

float target   = 5.0f;   // V, loaded from flash
float calVout  = 1.0f;   // multimeter correction factor, loaded from flash
float vout     = 0.0f;   // filtered, calibrated output voltage
float vbat     = 0.0f;   // filtered battery voltage
float voutRaw  = 0.0f;   // filtered, uncalibrated output voltage
bool  overRange = false;
bool  onTarget  = false;

enum Mode { MODE_RUN, MODE_CAL };
Mode mode = MODE_RUN;
float calWork = 1.0f;    // factor being dialed in during calibration

// ---------- Buzzer ----------
void beep(int freq, int ms) {
  tone(PIN_BUZZER, freq, ms);
}

Button btnUp{PIN_BTN_UP}, btnDown{PIN_BTN_DOWN}, btnOk{PIN_BTN_OK};

// ---------- Measuring ----------
int readMilliVolts(int pin) {
  long sum = 0;
  for (int i = 0; i < 32; i++) sum += analogReadMilliVolts(pin);
  return sum / 32;
}

void measure() {
  int mvOut = readMilliVolts(PIN_VOUT_SENSE);
  int mvBat = readMilliVolts(PIN_VBAT_SENSE);
  overRange = mvOut > ADC_MAX_MV;

  float newRaw = mvOut / 1000.0f * VOUT_RATIO;
  float newBat = mvBat / 1000.0f * VBAT_RATIO;

  // Smooth the reading, but jump straight to big changes so the display feels live.
  if (fabsf(newRaw - voutRaw) > 0.5f) voutRaw = newRaw;
  else voutRaw += (newRaw - voutRaw) * 0.3f;
  if (vbat == 0.0f) vbat = newBat;
  else vbat += (newBat - vbat) * 0.1f;

  vout = voutRaw * calVout;
}

// ---------- Drawing helpers ----------
void drawSmallTop(const char *left) {
  char buf[12];
  oled.setFont(u8g2_font_5x7_tr);
  oled.drawStr(0, 7, left);
  // Battery in the top right corner; blinks "LOW" when it needs charging.
  if (vbat < LOW_BATT_V && (millis() / 500) % 2) snprintf(buf, sizeof buf, "LOW");
  else snprintf(buf, sizeof buf, "%.1fV", vbat);
  oled.drawStr(72 - oled.getStrWidth(buf), 7, buf);
}

void drawBig(float v) {
  char buf[10];
  snprintf(buf, sizeof buf, "%.2f", v);
  oled.setFont(u8g2_font_logisoso16_tn);
  int w = oled.getStrWidth(buf);
  int x = (72 - w - 6) / 2;
  oled.drawStr(x, 27, buf);
  oled.setFont(u8g2_font_5x7_tr);
  oled.drawStr(x + w + 1, 27, "V");
}

void drawCentered(int y, const char *s) {
  oled.setFont(u8g2_font_5x7_tr);
  oled.drawStr((72 - oled.getStrWidth(s)) / 2, y, s);
}

// ---------- Screens ----------
void drawRun() {
  char buf[16];
  snprintf(buf, sizeof buf, "T%.2f", target);
  drawSmallTop(buf);

  if (overRange) {
    drawCentered(24, "OVER 27V");
    drawCentered(39, "TURN DOWN");
    return;
  }
  if (vout < NO_MODULE_V) {
    oled.setFont(u8g2_font_logisoso16_tn);
    oled.drawStr(14, 27, "--.--");
    drawCentered(39, "NO MODULE");
    return;
  }

  drawBig(vout);

  float diff = target - vout;   // positive: output too low, turn the pot up
  if (onTarget) {
    drawCentered(39, "ON TARGET");
  } else if (diff > 0) {
    oled.drawTriangle(2, 39, 10, 39, 6, 32);          // up arrow
    snprintf(buf, sizeof buf, "UP %.2f", diff);
    oled.setFont(u8g2_font_5x7_tr);
    oled.drawStr(14, 39, buf);
  } else {
    oled.drawTriangle(2, 32, 10, 32, 6, 39);          // down arrow
    snprintf(buf, sizeof buf, "DN %.2f", -diff);
    oled.setFont(u8g2_font_5x7_tr);
    oled.drawStr(14, 39, buf);
  }
}

void drawCal() {
  drawSmallTop("CAL");
  if (voutRaw < 1.0f || overRange) {
    drawCentered(24, "NEED >1V");
    drawCentered(39, "HOLD OK=RESET");
    return;
  }
  drawBig(voutRaw * calWork);
  drawCentered(39, "MATCH METER");
}

// ---------- Setup and loop ----------
void setup() {
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);
  pinMode(PIN_BTN_UP, INPUT_PULLUP);
  pinMode(PIN_BTN_DOWN, INPUT_PULLUP);
  pinMode(PIN_BTN_OK, INPUT_PULLUP);

  Serial.begin(115200);

  analogReadResolution(12);   // default 11 dB range: about 0-2.5 V on the ESP32-C3

  prefs.begin("jig", false);
  target  = prefs.getFloat("target", 5.0f);
  calVout = prefs.getFloat("calVout", 1.0f);
  if (calVout < 0.8f || calVout > 1.2f) calVout = 1.0f;   // ignore nonsense

  oled.begin();
  oled.setContrast(255);

  beep(2000, 60);
}

void loop() {
  bool upP, upR, dnP, dnR, okP, okR;
  updateButton(btnUp, upP, upR);
  updateButton(btnDown, dnP, dnR);
  updateButton(btnOk, okP, okR);

  measure();

  // OK held for 2 seconds: enter calibration (or, in calibration, reset it).
  bool okLong = false;
  if (btnOk.down && !btnOk.longFired && millis() - btnOk.pressedAt > 2000) {
    btnOk.longFired = true;
    okLong = true;
  }
  bool okShort = okR && !btnOk.longFired;

  if (mode == MODE_RUN) {
    if (repeatStep(btnUp, upP))   target += stepSize(btnUp, 0.1f, 0.5f);
    if (repeatStep(btnDown, dnP)) target -= stepSize(btnDown, 0.1f, 0.5f);
    target = constrain(target, TARGET_MIN, TARGET_MAX);
    target = roundf(target * 10.0f) / 10.0f;   // keep it on a 0.1 V grid

    if (okShort) {
      prefs.putFloat("target", target);
      beep(2500, 80);
    }
    if (okLong) {
      mode = MODE_CAL;
      calWork = calVout;
      beep(1500, 150);
    }

    // On-target beep, with a little hysteresis so it doesn't chatter.
    bool present = vout >= NO_MODULE_V && !overRange;
    float err = fabsf(target - vout);
    if (!onTarget && present && err <= TOLERANCE) {
      onTarget = true;
      beep(3000, 120);
    } else if (onTarget && (!present || err > TOLERANCE * 2)) {
      onTarget = false;
    }
  } else {  // MODE_CAL
    // Each step moves the shown voltage by 0.01 V (0.1 V when held), so it follows the pot live.
    if (voutRaw >= 1.0f) {
      if (repeatStep(btnUp, upP))   calWork += stepSize(btnUp, 0.01f, 0.1f) / voutRaw;
      if (repeatStep(btnDown, dnP)) calWork -= stepSize(btnDown, 0.01f, 0.1f) / voutRaw;
    }

    if (okShort && voutRaw >= 1.0f && !overRange) {
      float f = calWork;
      if (f >= 0.8f && f <= 1.2f) {
        calVout = f;
        prefs.putFloat("calVout", calVout);
        beep(2500, 80);
        delay(100);
        beep(3000, 80);
      } else {
        beep(400, 300);   // more than 20% off: something is wrong, not saved
      }
      mode = MODE_RUN;
    } else if (okShort) {
      mode = MODE_RUN;    // nothing to calibrate against, just leave
    }
    if (okLong) {
      calVout = 1.0f;
      prefs.putFloat("calVout", calVout);
      beep(800, 300);
      mode = MODE_RUN;
    }
  }

  oled.clearBuffer();
  if (mode == MODE_RUN) drawRun();
  else drawCal();
  oled.sendBuffer();

  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 500) {
    lastPrint = millis();
    Serial.printf("vout=%.3f raw=%.3f vbat=%.2f target=%.2f cal=%.4f\n",
                  vout, voutRaw, vbat, target, calVout);
  }

  delay(20);
}
