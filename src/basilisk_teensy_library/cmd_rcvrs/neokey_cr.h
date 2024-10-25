#pragma once

#include <Arduino.h>

#include "../basilisk.h"
#include "../components/neokey.h"
#include "../globals/serials.h"
#include "../helpers/beat.h"
#include "../helpers/serial_print.h"
#include "../tests/modes.h"

class NeokeyCommandReceiver {
 public:
  NeokeyCommandReceiver(Neokey& nk, Basilisk& b) : nk_{nk}, b_{b} {}

  // Should be called before use.
  bool Setup() {
    if (!nk_.Setup([this](uint16_t key) { Parse(key); })) {
#if DEBUG_SETUP
      Pln("NeokeyCommandReceiver: Neokey setup failed");
#endif
      return false;
    };

#if DEBUG_SETUP
    Pln("NeokeyCommandReceiver: Setup complete");
#endif
    return true;
  }

  void Parse(uint16_t key) {
    injection_ = key + 1;
    b_.crmux_ = Basilisk::CRMux::Neokey;

#if DEBUG_NEOKEYCR
    P("NeokeyCommandReceiver: Key rose: ");
    Serial.print(key);
    P(", injection_: ");
    Serial.println(injection_);
#endif
  }

  void Run() {
    if (!beat_.Hit()) return;
    nk_.Read();
  }

  void Inject() {
    using C = Basilisk::Command;
    using O = C::Oneshot;
    using M = C::Mode;
    static auto& c = b_.cmd_;
    static auto& m = c.mode;

    switch (injection_) {
      case 0: {
        // Last Neokey Command already processed, so don't touch.
      } break;
      case 1: {
        m = M::Idle_Init;
      } break;
      case 2: {
        m = M::Free;
      } break;
      case 3: {  // SetBaseYaw(0.0)
        c.oneshot = O::SetBaseYaw;
        c.set_base_yaw.offset = 0.0;
      } break;
      case 4: {
      } break;
      default: {
      } break;
    }
  }

  uint16_t injection_ = 0;

 private:
  Neokey& nk_;
  Basilisk& b_;
  Beat beat_{10};
};
