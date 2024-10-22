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

  void Receive(uint16_t key) {
    nk_cmd_ = key + 1;
    b_.crmux_ = Basilisk::CRMux::Neokey;

#if DEBUG_NEOKEYCR
    P("NeokeyCommandReceiver: Key rose: ");
    Serial.print(key);
    P(", nk_cmd_: ");
    Serial.println(nk_cmd_);
#endif
  }

  // Should be called before use.
  bool Setup() {
    if (!nk_.Setup([this](uint16_t key) { Receive(key); })) {
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

  void Run() {
    if (!beat_.Hit()) return;
    nk_.Read();
  }

  void Parse() {
    using M = Basilisk::Command::Mode;
    static auto& c = b_.cmd_;
    static auto& m = c.mode;

    switch (nk_cmd_) {
      case 0: {  // Last Neokey Command already processed, so don't touch.
      } break;
      case 1: {
        m = M::Idle_Init;
      } break;
      case 2: {
        m = M::Free;
      } break;
      case 3: {  // SetBaseYaw(0.0)
        c.oneshots |= (1 << 1);
        c.set_base_yaw.offset = 0.0;
      } break;
      case 4: {
        tests::SetPhis(b_);
        // b_.crmux_ = Basilisk::CRMux::Xbee;
        // tests::BounceWalk(b_);
        // tests::Pivot(b_);
        // tests::Diamond(b_);
        // tests::WalkToPosInField(b_);
      } break;
      default: {  // Whatever left keys are assigned Idle Mode.
        m = M::Idle_Init;
      } break;
    }
  }

  uint16_t nk_cmd_ = 0;

 private:
  Neokey& nk_;
  Basilisk& b_;
  Beat beat_{10};
};
