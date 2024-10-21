#pragma once

#include <Arduino.h>

#include "../helpers/clamped.h"
#include "../helpers/do_you_want_debug.h"
#include "../helpers/serial_print.h"
#include "../helpers/typedefs.h"
#include "../helpers/utils.h"

/* Stores the history of feet contact to groud to two uint64_t's,
 * each for each foot in `state_[0:2].contact`. MSB contains recent data,
 * and LSB oldest, 1 representing contact and 0 detachment. There are helper
 * methods in the state struct to verify meaningful contact/detachment.
 * Why named LegoBlocks? Cuz you scream when you step on them. */
class LegoBlocks {
 public:
  LegoBlocks(const int& pin_l = 23, const int& pin_r = 29)
      : pins_{pin_l, pin_r} {}

  // Must be called before use.
  bool Setup() {
    for (const auto& pin : pins_) pinMode(pin, INPUT);
#if DEBUG_PRINT_INITIALIZATION
    Pln("LegoBlocks: Setup complete");
#endif
    return true;
  }

  // Should be called in regular interval to track history of contact.
  void Run() {
    for (const int f : IDX_LR) {
      state_[f].contact >>= 1;
      if (digitalRead(pins_[f])) {
        state_[f].contact |= new_contact;
        state_[f].last_contact_time = millis();
      }
    }

    last_run_time = millis();
  }

  void Reset() {
    for (auto& ouch : state_) ouch.contact = 0;
  }

  const int pins_[2];
  struct {
    uint64_t contact = 0;
    uint32_t last_contact_time = 0;
    bool ConsecutiveContact(const N64& n) { return !(~contact >> (64 - n)); }
    bool ConsecutiveDetachment(const N64& n) { return !(contact >> (64 - n)); }
    int CountContact() {
      int cnt = 0;
      for (int i = 0; i < 64; i++) {
        if (contact & (one_uint64 << i)) cnt++;
      }
      return cnt;
    }
    bool ProbableContact(const N64& n) { return CountContact() >= n; }
    bool ProbableDetachment(const N64& n) { return 64 - CountContact() >= n; }
  } state_[2];
  uint32_t last_run_time = 0;

 private:
  inline static const uint64_t new_contact = one_uint64 << 63;
};
