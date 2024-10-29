#pragma once

#include <Arduino.h>

#include "../basilisk.h"
#include "../components/neokey.h"
#include "../globals/serials.h"
#include "../helpers/beat.h"
// #include "../tests/modes.h"

class NeokeyCommandReceiver {
 public:
  NeokeyCommandReceiver(Neokey& nk, Basilisk& b,
                        const uint32_t& run_interval = 10)
      : nk_{nk}, b_{b}, beat_{run_interval} {
    nk_.Setup([this](uint16_t key) { Parse(key); });
  }

  void Parse(uint16_t key) {
    injection_ = key + 1;
    b_.crmux_ = Basilisk::CRMux::Neokey;

#if DEBUG_NEOKEYCR
    P("NeokeyCommandReceiver: Key rose -> ");
    Serial.print(key);
    P(", injection -> ");
    Serial.println(injection_);
#endif
  }

  void Run() {
    if (!beat_.Hit()) return;
    nk_.Read();
  }

  void Inject() {
    using C = Basilisk::Command;
    using O = C::Oneshot::ByteRep;
    using M = C::Mode;
    static auto& c = b_.cmd_;
    static auto& m = c.mode;

    switch (injection_) {
      case 0: {
        // Last Neokey Command already processed, so don't touch.
      } break;
      case 1: {  // Idle Mode
        m = M::Idle_Init;
      } break;
      case 2: {  // Free Mode
        m = M::Free;
      } break;
      case 3: {  // SetBaseYaw(0.0)
        c.oneshots.Add(O::SetBaseYaw);
        c.set_base_yaw.offset = 0.0;
      } break;
      case 4: {  // Left for debug purposes.
        b_.CommandBoth([](Servo& s) {
          s.SetPosition([] {
            auto cmd = g::moteus_fmt::pm_cmd_template;
            cmd.position = NaN;
            cmd.velocity = 0.2;
            cmd.maximum_torque = 0.02;
            cmd.watchdog_timeout = NaN;
            return cmd;
          }());
        });
      } break;
      default: {
      } break;
    }
  }

  uint16_t injection_ = 0;

 private:
  Neokey& nk_;
  Basilisk& b_;
  Beat beat_;
};
