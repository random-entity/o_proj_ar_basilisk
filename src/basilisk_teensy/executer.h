#pragma once

#include "basilisk.h"
// #include "cmd_rcvrs/neokey_cr.h"
// #include "cmd_rcvrs/xbee_cr.h"
// #include "mode_runners/_matome.h"
// #include "oneshots/matome.h"

class Executer {
 public:
  Executer(Basilisk* b) : b_{b} {}

  void Run() {
    // if (XbeeCommandReceiver::waiting_parse_) {
    //   if (XbeeCommandReceiver::xb_cmd_.decoded.oneshots &
    //       (1 << ONESHOT_CRMuxXbee)) {
    //     b_->cmd_.oneshots |= (1 << ONESHOT_CRMuxXbee);
    //     XbeeCommandReceiver::waiting_parse_ = false;
    //   }
    //   if (XbeeCommandReceiver::xb_cmd_.decoded.mode ==
    //           static_cast<uint8_t>(Basilisk::Command::Mode::DoPreset) &&
    //       XbeeCommandReceiver::xb_cmd_.decoded.u.do_preset
    //               .idx[b_->cfg_.suid - 1] == 50002) {
    //     b_->cmd_.oneshots |= (1 << ONESHOT_CRMuxXbee);
    //   }
    // }

    // BasiliskOneshots::Shoot(b_);

    b_->CommandBoth([](Servo* s) {
      const auto got_rpl = s->SetQuery();
      // Pln((got_rpl ? "G" : "n"));
      // Pln((s->failure_.Exists() ? "F" : "nn"));
      // Serial.println(s->failure_.Export(), BIN);
    });

    // switch (b_->crmux_) {
    //   case Basilisk::CRMux::Neokey: {
    //     if (NeokeyCommandReceiver::nk_cmd_ != 0) {
    //       NeokeyCommandReceiver::Parse();
    //       NeokeyCommandReceiver::nk_cmd_ = 0;
    //     }
    //   } break;
    //   case Basilisk::CRMux::Xbee: {
    //     if (XbeeCommandReceiver::waiting_parse_) {
    //       XbeeCommandReceiver::Parse();
    //       XbeeCommandReceiver::waiting_parse_ = false;
    //     }
    //   } break;
    //   default:
    //     break;
    // }

    // for (uint8_t id = 0; id < 4; id++) {
    //   if (b_->mags_.heavenfall_[id]) {
    //     P("Heavenfall ");
    //     Serial.println(id);

    //     b_->cmd_.mode = Basilisk::Command::Mode::Idle_Init;
    //     break;
    //   }
    // }

    // if (b_->l_.GetReply().torque > 20.0 || b_->r_.GetReply().torque > 20.0) {
    //   b_->cmd_.mode = Basilisk::Command::Mode::Idle_Init;
    // }

    // auto* maybe_mode_runner = SafeAt(ModeRunners::mode_runners,
    // b_->cmd_.mode); if (maybe_mode_runner) {
    //   (*maybe_mode_runner)(b_);
    // } else {
    //   // Pln("Mode NOT registered to ModeRunners::mode_runners");
    // }
  }

 private:
  Basilisk* b_;
};
