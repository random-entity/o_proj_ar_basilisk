#pragma once

#include "../presets/_matome.h"
#include "../rpl_sndrs/led_rs.h"
#include "_meta.h"
#include "../basilisk.h"
#include "../cmd_rcvrs/xbee_cr.h"

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

      if (idx == 70) {  // 9   10   11   12   13
                        // 5      6     7      8
                        // 1      2     3      4
        static const auto& suid = b.cfg_.suid;

        m = M::WalkToPosInField_Init;
        auto& c = b.cmd_.walk_to_pos_in_field;

        static const uint8_t row = suid <= 4 ? 0 : suid <= 8 ? 1 : 2;
        static const uint8_t col = suid <= 12 ? (suid - 1) % 4 : 4;
        static double x =
            row < 2
                ? (b.cfg_.lps.minx * (3 - col) + b.cfg_.lps.maxx * col) / 3.0
                : (b.cfg_.lps.minx * (4 - col) + b.cfg_.lps.maxx * col) / 4.0;
        static double y =
            (b.cfg_.lps.miny * (2 - row) + b.cfg_.lps.maxy * row) / 2.0;
        c.tgt_pos = Vec2{x, y};

        return;
      }

      if (idx == 71) {  // 7    6   13    4    3
                        //    12  11   9   10
                        //      8    5    2
                        //           1
        static const auto& suid = b.cfg_.suid;

        m = M::WalkToPosInField_Init;
        auto& c = b.cmd_.walk_to_pos_in_field;

        c.tgt_pos = suid == 1    ? Vec2{400, 150}
                    : suid == 8  ? Vec2{250, 300}
                    : suid == 5  ? Vec2{400, 300}
                    : suid == 2  ? Vec2{550, 300}
                    : suid == 12 ? Vec2{200, 500}
                    : suid == 11 ? Vec2{350, 500}
                    : suid == 9  ? Vec2{500, 500}
                    : suid == 10 ? Vec2{650, 500}
                    : suid == 7  ? Vec2{100, 750}
                    : suid == 6  ? Vec2{250, 750}
                    : suid == 13 ? Vec2{400, 750}
                    : suid == 4  ? Vec2{550, 750}
                    : suid == 3  ? Vec2{700, 750}
                                 : Vec2{425, 425};

        return;
      }

      if (idx == 72) {  // 7 8  9 10 12
                        // 3 4      5 6
                        // 1    11    2
        static const auto& suid = b.cfg_.suid;
        m = M::WalkToPosInField_Init;
        auto& c = b.cmd_.walk_to_pos_in_field;
        c.tgt_pos = suid == 1    ? Vec2{150, 250}
                    : suid == 2  ? Vec2{750, 250}
                    : suid == 3  ? Vec2{150, 500}
                    : suid == 4  ? Vec2{300, 500}
                    : suid == 5  ? Vec2{550, 500}
                    : suid == 6  ? Vec2{750, 500}
                    : suid == 7  ? Vec2{150, 750}
                    : suid == 8  ? Vec2{300, 750}
                    : suid == 9  ? Vec2{450, 750}
                    : suid == 10 ? Vec2{600, 750}
                    : suid == 12 ? Vec2{750, 750}
                    : suid == 11 ? Vec2{425, 125}
                                 : Vec2{425, 425};

        return;
      }

      if (idx == 73) {
        m = M::Sufi;
        auto& c = b.cmd_.sufi;

        static const auto solo = Vec2{420, 125};

        c.init_didimbal = BOOL_L;
        c.dest_yaw =
            nearest_pmn(b.imu_.GetYaw(true), (solo - b.lps_.GetPos()).arg());
        c.exit_thr = 0.01;
        c.stride = 30.0 / 360.0;
        bool dest_is_greater = c.dest_yaw > b.imu_.GetYaw(true);
        if (!dest_is_greater) {
          c.stride *= -1.0;
        }
        c.bend[IDX_L] = 0.0;
        c.bend[IDX_L] = 0.0;
        c.speed = g::c::speed::normal;
        c.acclim = g::c::acclim::standard;
        c.min_stepdur = 0;
        c.max_stepdur = g::c::maxdur::safe;
        c.interval = 100;
        c.steps = -1;

        return;
      }

      if (81 <= idx && idx <= 88) {  // mod 10 == SENW where SUID 1, 9 at
        uint8_t digits[4];
        for (uint8_t i = 0; i < 4; i++) {
          digits[i] = idx % 10;
          idx /= 10;
        }

        m = M::WalkToPosInField_Init;
        auto& c = b.cmd_.walk_to_pos_in_field;

        static const auto center =
            Vec2{(b.cfg_.lps.minx + b.cfg_.lps.maxx) * 0.5,
                 (b.cfg_.lps.miny + b.cfg_.lps.maxy) * 0.5};

        if (b.cfg_.suid <= 8) {
          double arg = (digits[0] + b.cfg_.suid - 4) * 0.125;
          c.tgt_pos = center + 320.0 * Vec2{arg};
        } else if (b.cfg_.suid <= 12) {
          double arg = (digits[0] + b.cfg_.suid * 2 - 21) * 0.125;
          c.tgt_pos = center + 160.0 * Vec2{arg};
        } else {
          c.tgt_pos = center;
        }

        return;
      }

      if (101 <= idx && idx <= 108) {
        uint8_t digits[4];
        for (uint8_t i = 0; i < 4; i++) {
          digits[i] = idx % 10;
          idx /= 10;
        }

        m = M::WalkToDir;
        auto& c = b.cmd_.walk_to_dir;

        static const auto center =
            Vec2{(b.cfg_.lps.minx + b.cfg_.lps.maxx) * 0.5,
                 (b.cfg_.lps.miny + b.cfg_.lps.maxy) * 0.5};

        const auto cur_yaw = b.imu_.GetYaw(true);
        c.init_didimbal = BOOL_L;
        c.tgt_yaw = nearest_pmn(cur_yaw, cur_yaw + ((digits[0] - 1) / 8.0));
        c.stride = 30.0 / 360.0;
        if (abs(c.tgt_yaw - cur_yaw) > 0.25) {
          c.tgt_yaw = nearest_pmn(cur_yaw, c.tgt_yaw + 0.5);
          c.stride *= -1.0;
        }
        c.bend[IDX_L] = 0.0;
        c.bend[IDX_L] = 0.0;
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
