#pragma once

#include <Arduino.h>
#include <elapsedMillis.h>

#include "../globals/serials.h"
#include "../helpers/beat.h"
#include "../helpers/clamped.h"
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
  LegoBlocks(const int& pin_l = 23, const int& pin_r = 29,
             const uint32_t& run_interval = 20)
      : pins_{pin_l, pin_r}, beat_{run_interval} {}

  // Must be called before use.
  bool Setup() {
    for (const auto& pin : pins_) pinMode(pin, INPUT);
#if DEBUG_SETUP
    Pln("LegoBlocks: Setup complete");
#endif
    return true;
  }

  // Call continuously to track history of contact.
  void Run() {
    if (!beat_.Hit()) return;

    for (const int f : IDX_LR) {
      state_[f].contact >>= 1;
      if (digitalRead(pins_[f])) {
        state_[f].contact |= new_contact;
        state_[f].since_contact = 0;
      }
    }
  }

  void Reset() {
    for (auto& ouch : state_) ouch.contact = 0;
  }

  const int pins_[2];
  struct {
    uint64_t contact = 0;
    elapsedMillis since_contact = 0;
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

 private:
  Beat beat_;
  inline static constexpr uint64_t new_contact = one_uint64 << 63;
};
