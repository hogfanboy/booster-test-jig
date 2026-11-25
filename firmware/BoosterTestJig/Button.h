#pragma once
// Kept in its own file so the Arduino IDE's automatic function declarations
// don't get placed before the Button type exists.
#include <Arduino.h>

// ---------- Buttons with debounce and hold-to-repeat ----------
struct Button {
  int pin;
  bool down = false;          // debounced state
  bool lastRaw = false;
  unsigned long changedAt = 0;
  unsigned long pressedAt = 0;
  unsigned long nextRepeat = 0;
  bool longFired = false;
};

// Updates the button. Returns:
//   pressed: true once when it goes down
//   released: true once when it comes back up
inline void updateButton(Button &b, bool &pressed, bool &released) {
  pressed = released = false;
  bool raw = digitalRead(b.pin) == LOW;
  unsigned long now = millis();
  if (raw != b.lastRaw) {
    b.lastRaw = raw;
    b.changedAt = now;
  }
  if (now - b.changedAt > 25 && raw != b.down) {
    b.down = raw;
    if (b.down) {
      pressed = true;
      b.pressedAt = now;
      b.nextRepeat = now + 500;  // first repeat after half a second
      b.longFired = false;
    } else {
      released = true;
    }
  }
}

// For UP / DOWN: true on press and then repeatedly while held.
inline bool repeatStep(Button &b, bool pressed) {
  if (pressed) return true;
  if (b.down && millis() >= b.nextRepeat) {
    b.nextRepeat = millis() + 120;
    return true;
  }
  return false;
}

// Bigger steps after holding for 2 seconds.
inline float stepSize(Button &b, float small, float big) {
  return (b.down && millis() - b.pressedAt > 2000) ? big : small;
}

