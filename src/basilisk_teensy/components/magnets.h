#pragma once

#include <Arduino.h>
#include <elapsedMillis.h>

#include "../globals/consts.h"
#include "../globals/serials.h"
#include "../helpers/serial_print.h"
#include "lego_blocks.h"

enum class MagStren : int {
  Max = 0,
  Strong = 63,
  Medium = 127,
  Weak = 191,
  Min = 255
};

MagStren Bool2MS(const bool attach) {
  return attach ? MagStren::Max : MagStren::Min;
}

// Part | LeftAnkle | LeftToe | RightAnkle | RightToe
// ID   | 0         | 1       | 2          | 3
// Pin  | 3         | 4       | 5          | 6

class Magnets {
 public:
  Magnets(LegoBlocks& lego,                              //
          const int& pin_la = 3, const int& pin_lt = 4,  //
          const int& pin_ra = 5, const int& pin_rt = 6)
      : pins_{pin_la, pin_lt, pin_ra, pin_rt}, lego_{lego} {}

  // Must be called before use.
  bool Setup() {
    for (const auto& pin : pins_) pinMode(pin, OUTPUT);
    AttachAll();
#if DEBUG_SETUP
    Pln("Magnets: Setup complete");
#endif
    return true;
  }

  // Should be called in regular interval to track if any of the
  // electromagnets are being passed current for over 3 seconds.
  void Run() {
    for (int id = 0; id < 4; id++) {
      if (attaching_[id]) {
        since_attach_[id] = 0;
      } else {
        since_release_[id] = 0;
      }
      heavenfall_[id] = (since_attach_[id] > consts::maxdur::yoyuu);
    }
  }

  void SetStrength(const int& id, const MagStren& strength) {
    analogWrite(pins_[id], static_cast<int>(strength));
    attaching_[id] = (strength == MagStren::Max);
    lego_.Reset();
  }

  void AttachAll() {
    for (int id = 0; id < 4; id++) {
      SetStrength(id, MagStren::Max);
    }
  }

  void ReleaseAll() {
    for (int id = 0; id < 4; id++) {
      SetStrength(id, MagStren::Min);
    }
  }

  const int pins_[4];
  bool attaching_[4] = {false};
  elapsedMillis since_attach_[4] = {0};
  elapsedMillis since_release_[4] = {0};
  bool heavenfall_[4] = {false};
  LegoBlocks& lego_;
};
