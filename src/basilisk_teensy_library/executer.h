#pragma once

#include "basilisk.h"
#include "cmd_rcvrs/neokey_cr.h"
#include "cmd_rcvrs/xbee_cr.h"
#include "mode_runners/_matome.h"
#include "oneshots/shoot.h"

class Executer {
 public:
  Executer(Basilisk& b, NeokeyCommandReceiver& nkcr, XbeeCommandReceiver& xbcr,
           const uint32_t& run_interval = 10)
      : b_{b}, xbcr_{xbcr}, nkcr_{nkcr}, beat_{run_interval} {}

  // Query.
  void Run() {
    using M = Basilisk::Command::Mode;
    static auto& m = b_.cmd_.mode;

    if (!beat_.Hit()) return;

    // Query.
    b_.CommandBoth([](Servo& s) { s.SetQuery(); });

    // Inject.
    switch (b_.crmux_) {
      case Basilisk::CRMux::Neokey: {
        if (nkcr_.injection_ != 0) {
          nkcr_.Inject();
          nkcr_.injection_ = 0;
        }
      } break;
      case Basilisk::CRMux::Xbee: {
        if (xbcr_.injection_.waiting) {
          xbcr_.Inject();
          xbcr_.injection_.waiting = false;
        }
      } break;
    }

    // Oneshot.
    Shoot(b_);

    // Handle failures right before Mode running.
    if (b_.rpl_.heavenfall()) {
      P("Heavenfall ");
      Serial.printf("0x%04X\n", b_.rpl_.heavenfall(), HEX);
      m = M::Idle_Init;
    }
    if (b_.l_.GetReply().torque > 0.4 || b_.r_.GetReply().torque > 0.4) {
      m = M::Idle_Init;
    }
    /* TODO: Develop */

    // Run Mode.
    mr_.mode_runners.at(m)();
  }

 private:
  Basilisk& b_;
  ModeRunners mr_{b_};
  XbeeCommandReceiver& xbcr_;
  NeokeyCommandReceiver& nkcr_;
  Beat beat_;
};
