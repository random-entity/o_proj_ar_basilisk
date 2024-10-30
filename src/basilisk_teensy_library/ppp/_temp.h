#pragma once

#include "../basilisk.h"
#include "../cmd_rcvrs/xbee_cr.h"
#include "../presets/_matome.h"
#include "../rpl_sndrs/led_rs.h"
#include "_meta.h"

namespace presets::globalvar {
PhiSpeed speed;
}

struct Presets {
  using M = Basilisk::Command::Mode;

  inline static void Diamond(Basilisk* b, LR init_didimbal) {
    auto& m = b.cmd_.mode;
    auto& c = b.cmd_.diamond;

    m = M::Diamond;
    c.init_didimbal = init_didimbal;
    c.init_stride = 0.3;
    c.speed = globals::stdval::speed::normal;
    c.acclim = globals::stdval::acclim::standard;
    c.min_stepdur = 0;
    c.max_stepdur = -1;
    c.interval = 0;
    c.steps = -1;
  }

  inline static const std::map<uint16_t, void (*)(Basilisk*)> presets = {
      // Specific
      {50, [](Basilisk* b) { Diamond(b, BOOL_L); }},
      {51, [](Basilisk* b) { Diamond(b, BOOL_R); }},
  };
};

void ModeRunners::BPPP(Basilisk* b) {
  auto& m = b.cmd_.mode;
  auto idx = b.cmd_.ppp.idx;  // Copy, not reference.

  P("Entered BPPP Mode, Preset idx ");
  Serial.println(idx);

  switch (m) {
    case M::BPPP: {
      if (10000 <= idx && idx <= 19999) {  // PPP WalkToPos range
                                           // Decimal 1ABCD
                                           // (AB) = tgt_pos_x
                                           //          == (AB) * 10cm
                                           // (CD) = tgt_pos_y
                                           //          == (CD) * 10cm
        uint8_t digits[4];
        for (uint8_t i = 0; i < 4; i++) {
          digits[i] = idx % 10;
          idx /= 10;
        }

        m = M::WalkToPos;
        auto& c = b.cmd_.walk_to_pos;
        c.init_didimbal = BOOL_L;
        double x = (10 * digits[3] + digits[2]) * 10.0;
        double y = (10 * digits[1] + digits[0]) * 10.0;
        c.tgt_pos = Vec2{x, y};
        c.dist_thr = 25;
        c.stride = 0.125;
        for (uint8_t f : IDX_LR) c.bend[f] = 0.0;
        c.speed = g::c::speed::normal;
        c.acclim = g::c::acclim::standard;
        c.min_stepdur = 0;
        c.max_stepdur = g::c::maxdur::safe;
        c.interval = 0;
        c.steps = -1;

        return;
      }

      if (201 <= idx && idx <= 213) {
        const auto finding_suid = idx - 200;
        if (b.cfg_.suid == finding_suid) {
          led_rs::finding_me = true;
        }

        m = b.cmd_.ppp.prev_mode;
        b.cmd_.ppp.idx = 0;

        return;
      }

      // TODO: PPP for other Modes.

      auto* maybe_preset = SafeAt(Presets::presets, idx);
      if (maybe_preset) {
        (*maybe_preset)(b);
      } else if (idx != 0) {
        Pln("Unregistered Preset index");
        // m = M::SetMags_Init;
        // for (uint8_t i = 0; i < 4; i++) {
        //   b.cmd_.set_mags.strengths[i] = MagStren::Min;
        // }
        // for (uint8_t i = 0; i < 2; i++) {
        //   b.cmd_.set_mags.expected_state[i] = BOOL_RELEASE;
        // }
        // b.cmd_.set_mags.verif_thr = 1;
        // b.cmd_.set_mags.min_dur = 0;
        // b.cmd_.set_mags.max_dur = 100;
        // b.cmd_.set_mags.exit_to_mode = M::Idle_Init;
      }
    } break;
    default:
      break;
  }
}
