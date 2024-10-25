#pragma once

#include "../presets/_matome.h"
#include "../rpl_sndrs/led_rs.h"
#include "_meta.h"

// * Meta
// 50000 Idle
// 50001 Free
// 50002 CRMuxXbee
// 50003 SetBaseYaw(0)
// 50004 SetBaseYaw(-0.25)

// * Specific
// 1 ~ 4 Tibu foot
// 5 ~ 12 Tibu individual magnet ###TODO###
// 5 ~ 16 Pivot nogadas (deprecated)
// 23 ~ 24 RandomTibutibu
// 30 ~ 34 Change global SetPhis speed
// 50 ~ 51 Diamond
// 99 BounceWalk(tgt_yaw = random)

// * PPP
// 60 ~ 69 Set Pivot Tadak interval ###TODO###
// 70 WalkToPosInField to grid arrangement
// 71 WalkToPosInField to pyramid arrangement
// 72 WalkToPosInField to solo-chorus arrangement
// 73 Sufi to look at solo
// 81 ~ 88 WalkToPosInField to '1-4-8' circular arrangement
// 91 ~ 98 Sufi to target yaw relative to center
// 101 ~ 108 WalkToDir relative to current yaw
// 201 ~ 213 Find SUID
// 701 ~ 713 WalkToPosInField to '0-13' circular arrangement
// 1000 ~ 2999 Pivot
// 3100 ~ 3299 PivSpin
// 3300 ~ 3399 Sufi
// 4000 ~ 4999 WalkToDir
// 10000 ~ 19999 WalkToPos
// 20000 ~ 29999 WalkToPosInField

namespace do_preset {

namespace pivot {
double tgt_yaw;
PhiSpeed speed = globals::stdval::speed::normal;
}  // namespace pivot

namespace piv_spin {
PhiSpeed speed = globals::stdval::speed::normal;
}  // namespace piv_spin

}  // namespace do_preset

void ModeRunners::BPPP(Basilisk* b) {
  auto& m = b->cmd_.mode;
  auto idx = b->cmd_.do_preset.idx;  // Copy, not reference.

  P("Entered BPPP Mode, Preset idx ");
  Serial.println(idx);

  switch (m) {
    case M::BPPP: {
      if (1000 <= idx && idx <= 2999) {  // PPP Pivot range
                                         // Decimal ABCD
                                         // A = didim = 1 ~ 2
                                         //       == 1 -> L
                                         //       == 2 -> R
                                         // B = tgt_yaw = 1 ~ 8
                                         //       == 1 ~ 8 -> S, SE, E, NE, ...
                                         // C = bend_l = 1 ~ 6
                                         //       == 1 -> Straight
                                         //       == 2 -> Palja
                                         //       == 3 -> Morasou
                                         //       == 4 -> Outward
                                         //       == 5 -> Awkward
                                         // D = bend_r
                                         //       == Same as C
        uint8_t digits[4];
        for (uint8_t i = 0; i < 4; i++) {
          digits[i] = idx % 10;
          idx /= 10;
        }

        m = M::Pivot_Init;
        auto& c = b->cmd_.pivot;
        c.didimbal = digits[3] == 1 ? BOOL_L : BOOL_R;
        do_preset::pivot::tgt_yaw = (digits[2] - 3) * 0.125;
        c.tgt_yaw = [](Basilisk* b) {
          const auto cur_yaw = b->imu_.GetYaw(true);
          auto ty = nearest_pmn(cur_yaw, do_preset::pivot::tgt_yaw);
          if (b->cmd_.pivot.didimbal == BOOL_L &&
              ty - cur_yaw < -(0.375 + 0.0625)) {
            ty += 1.0;
          } else if (b->cmd_.pivot.didimbal == BOOL_R &&
                     ty - cur_yaw > 0.375 + 0.0625) {
            ty -= 1.0;
          }
          return ty;
        };
        c.stride = 0.0;
        c.bend[IDX_L] = digits[1] == 1   ? 0.0
                        : digits[1] == 2 ? 0.125
                        : digits[1] == 3 ? -0.125
                        : digits[1] == 4 ? 0.25
                        : digits[1] == 5 ? -0.25
                                         : 0.0;
        c.bend[IDX_R] = digits[0] == 1   ? 0.0
                        : digits[0] == 2 ? -0.125
                        : digits[0] == 3 ? 0.125
                        : digits[0] == 4 ? -0.25
                        : digits[0] == 5 ? 0.25
                                         : 0.0;
        c.speed = do_preset::pivot::speed;
        c.acclim = globals::stdval::acclim::standard;
        c.min_dur = 0;
        c.max_dur = globals::stdval::maxdur::safe;
        c.exit_condition = nullptr;
        c.exit_to_mode = M::Idle_Init;

        return;
      }

      if (3100 <= idx && idx <= 3299) {  // PPP PivSpin range
                                         // Decimal 3ABC
                                         // A = didim = 1 ~ 2
                                         //       == 1 -> L
                                         //       == 2 -> R
                                         // B = dest_yaw = 0 ~ 8
                                         //       == 1 ~ 8 -> S, SE, E, NE, ...
                                         //       == 0     -> NaN
                                         // C = stride = 0 ~ 9
                                         //       == 1 ~ 9 -> * 10 deg
                                         //       == 0     -> 45 deg
        uint8_t digits[3];
        for (uint8_t i = 0; i < 3; i++) {
          digits[i] = idx % 10;
          idx /= 10;
        }

        m = M::PivSpin;
        auto& c = b->cmd_.piv_spin;
        c.didimbal = digits[2] == 1 ? BOOL_L : BOOL_R;
        c.dest_yaw = digits[1] == 0 ? NaN
                                    : nearest_pmn(b->imu_.GetYaw(true),
                                                  (digits[1] - 3) * 0.125);
        c.exit_thr = 0.01;
        c.stride = digits[0] == 0 ? 0.125 : digits[0] / 36.0;
        if (!isnan(c.dest_yaw)) {
          bool dest_is_greater = c.dest_yaw > b->imu_.GetYaw(true);
          bool positive_stride_drives_greater = c.didimbal == BOOL_L;
          if (dest_is_greater != positive_stride_drives_greater) {
            c.stride *= -1.0;
          }
        }
        for (uint8_t f : IDX_LR) c.bend[f] = 0.0;
        c.speed = do_preset::piv_spin::speed;
        c.acclim = globals::stdval::acclim::standard;
        c.min_stepdur = 0;
        c.max_stepdur = globals::stdval::maxdur::safe;
        c.interval = 0;
        c.steps = -1;

        return;
      }

      if (3300 <= idx && idx <= 3399) {  // PPP Sufi range
                                         // Decimal 33AB
                                         // A = dest_yaw = 0 ~ 8
                                         //       == 0     -> NaN
                                         //       == 1 ~ 8 -> S, SE, E, NE, ...
                                         // B = stride = 0 ~ 9
                                         //       == 0     -> 45 deg
                                         //       == 1 ~ 9 -> * 10 deg
        uint8_t digits[2];
        for (uint8_t i = 0; i < 2; i++) {
          digits[i] = idx % 10;
          idx /= 10;
        }

        m = M::Sufi;
        auto& c = b->cmd_.sufi;
        c.init_didimbal = BOOL_L;
        c.dest_yaw = digits[1] == 0 ? NaN
                                    : nearest_pmn(b->imu_.GetYaw(true),
                                                  (digits[1] - 3) * 0.125);
        c.exit_thr = 0.01;
        c.stride = digits[0] == 0 ? 0.125 : digits[0] / 36.0;
        if (!isnan(c.dest_yaw)) {
          bool dest_is_greater = c.dest_yaw > b->imu_.GetYaw(true);
          if (!dest_is_greater) {
            c.stride *= -1.0;
          }
        }
        for (uint8_t f : IDX_LR) c.bend[f] = 0.0;
        c.speed = do_preset::piv_spin::speed;
        c.acclim = globals::stdval::acclim::standard;
        c.min_stepdur = 0;
        c.max_stepdur = globals::stdval::maxdur::safe;
        c.interval = 100;
        c.steps = -1;

        return;
      }

      if (4000 <= idx && idx <= 4999) {  // PPP WalkToDir range
                                         // Decimal 4ABC
                                         // A = tgt_yaw = 0 ~ 8
                                         //       == 0     -> NaN
                                         //       == 1 ~ 8 -> NSEW
                                         // B = stride = 0 ~ 9
                                         //       == 0     -> 45 deg
                                         //       == 1 ~ 9 -> * 10 deg
                                         // C = steps
                                         //       == 0     -> 10
                                         //       == 1 ~ 9 -> =
        uint8_t digits[3];
        for (uint8_t i = 0; i < 3; i++) {
          digits[i] = idx % 10;
          idx /= 10;
        }

        m = M::WalkToDir;
        auto& c = b->cmd_.walk_to_dir;
        c.init_didimbal = BOOL_L;
        c.tgt_yaw = digits[2] == 0 ? NaN
                                   : nearest_pmn(b->imu_.GetYaw(true),
                                                 (digits[2] - 3) * 0.125);
        c.stride = digits[1] == 0 ? 0.125 : digits[1] / 36.0;
        if (abs(c.tgt_yaw - b->imu_.GetYaw(true)) > 0.25) {
          c.tgt_yaw = nearest_pmn(b->imu_.GetYaw(true), c.tgt_yaw + 0.5);
          c.stride *= -1.0;
        }
        for (uint8_t f : IDX_LR) c.bend[f] = 0.0;
        c.speed = globals::stdval::speed::normal;
        c.min_stepdur = 0;
        c.max_stepdur = globals::stdval::maxdur::safe;
        c.steps = digits[0] == 0 ? 10 : digits[0];

        return;
      }

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
        auto& c = b->cmd_.walk_to_pos;
        c.init_didimbal = BOOL_L;
        double x = (10 * digits[3] + digits[2]) * 10.0;
        double y = (10 * digits[1] + digits[0]) * 10.0;
        c.tgt_pos = Vec2{x, y};
        c.dist_thr = 25;
        c.stride = 0.125;
        for (uint8_t f : IDX_LR) c.bend[f] = 0.0;
        c.speed = globals::stdval::speed::normal;
        c.acclim = globals::stdval::acclim::standard;
        c.min_stepdur = 0;
        c.max_stepdur = globals::stdval::maxdur::safe;
        c.interval = 0;
        c.steps = -1;

        return;
      }

      if (20000 <= idx && idx <= 29999) {  // PPP WalkToPosInField range
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

        m = M::WalkToPosInField_Init;
        auto& c = b->cmd_.walk_to_pos_in_field;
        double x = (10 * digits[3] + digits[2]) * 10.0;
        double y = (10 * digits[1] + digits[0]) * 10.0;
        c.tgt_pos = Vec2{x, y};

        return;
      }

      if (idx == 70) {  // 9   10   11   12   13
                        // 5      6     7      8
                        // 1      2     3      4
        static const auto& suid = b->cfg_.suid;

        m = M::WalkToPosInField_Init;
        auto& c = b->cmd_.walk_to_pos_in_field;

        static const uint8_t row = suid <= 4 ? 0 : suid <= 8 ? 1 : 2;
        static const uint8_t col = suid <= 12 ? (suid - 1) % 4 : 4;
        static double x =
            row < 2
                ? (b->cfg_.lps.minx * (3 - col) + b->cfg_.lps.maxx * col) / 3.0
                : (b->cfg_.lps.minx * (4 - col) + b->cfg_.lps.maxx * col) / 4.0;
        static double y =
            (b->cfg_.lps.miny * (2 - row) + b->cfg_.lps.maxy * row) / 2.0;
        c.tgt_pos = Vec2{x, y};

        return;
      }

      if (idx == 71) {  // 7    6   13    4    3
                        //    12  11   9   10
                        //      8    5    2
                        //           1
        static const auto& suid = b->cfg_.suid;

        m = M::WalkToPosInField_Init;
        auto& c = b->cmd_.walk_to_pos_in_field;

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
        static const auto& suid = b->cfg_.suid;
        m = M::WalkToPosInField_Init;
        auto& c = b->cmd_.walk_to_pos_in_field;
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
        auto& c = b->cmd_.sufi;

        static const auto solo = Vec2{420, 125};

        c.init_didimbal = BOOL_L;
        c.dest_yaw =
            nearest_pmn(b->imu_.GetYaw(true), (solo - b->lps_.GetPos()).arg());
        c.exit_thr = 0.01;
        c.stride = 30.0 / 360.0;
        bool dest_is_greater = c.dest_yaw > b->imu_.GetYaw(true);
        if (!dest_is_greater) {
          c.stride *= -1.0;
        }
        c.bend[IDX_L] = 0.0;
        c.bend[IDX_L] = 0.0;
        c.speed = globals::stdval::speed::normal;
        c.acclim = globals::stdval::acclim::standard;
        c.min_stepdur = 0;
        c.max_stepdur = globals::stdval::maxdur::safe;
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
        auto& c = b->cmd_.walk_to_pos_in_field;

        static const auto center =
            Vec2{(b->cfg_.lps.minx + b->cfg_.lps.maxx) * 0.5,
                 (b->cfg_.lps.miny + b->cfg_.lps.maxy) * 0.5};

        if (b->cfg_.suid <= 8) {
          double arg = (digits[0] + b->cfg_.suid - 4) * 0.125;
          c.tgt_pos = center + 320.0 * Vec2{arg};
        } else if (b->cfg_.suid <= 12) {
          double arg = (digits[0] + b->cfg_.suid * 2 - 21) * 0.125;
          c.tgt_pos = center + 160.0 * Vec2{arg};
        } else {
          c.tgt_pos = center;
        }

        return;
      }

      if (91 <= idx && idx <= 98) {
        uint8_t digits[4];
        for (uint8_t i = 0; i < 4; i++) {
          digits[i] = idx % 10;
          idx /= 10;
        }

        m = M::Sufi;
        auto& c = b->cmd_.sufi;

        static const auto center =
            Vec2{(b->cfg_.lps.minx + b->cfg_.lps.maxx) * 0.5,
                 (b->cfg_.lps.miny + b->cfg_.lps.maxy) * 0.5};

        c.init_didimbal = BOOL_L;
        c.dest_yaw = nearest_pmn(
            b->imu_.GetYaw(true),
            (center - b->lps_.GetPos()).arg() + ((digits[0] - 1) / 8.0));
        c.exit_thr = 0.01;
        c.stride = 30.0 / 360.0;
        bool dest_is_greater = c.dest_yaw > b->imu_.GetYaw(true);
        if (!dest_is_greater) {
          c.stride *= -1.0;
        }
        c.bend[IDX_L] = 0.0;
        c.bend[IDX_L] = 0.0;
        c.speed = globals::stdval::speed::normal;
        c.acclim = globals::stdval::acclim::standard;
        c.min_stepdur = 0;
        c.max_stepdur = globals::stdval::maxdur::safe;
        c.interval = 100;
        c.steps = -1;

        return;
      }

      if (101 <= idx && idx <= 108) {
        uint8_t digits[4];
        for (uint8_t i = 0; i < 4; i++) {
          digits[i] = idx % 10;
          idx /= 10;
        }

        m = M::WalkToDir;
        auto& c = b->cmd_.walk_to_dir;

        static const auto center =
            Vec2{(b->cfg_.lps.minx + b->cfg_.lps.maxx) * 0.5,
                 (b->cfg_.lps.miny + b->cfg_.lps.maxy) * 0.5};

        const auto cur_yaw = b->imu_.GetYaw(true);
        c.init_didimbal = BOOL_L;
        c.tgt_yaw = nearest_pmn(cur_yaw, cur_yaw + ((digits[0] - 1) / 8.0));
        c.stride = 30.0 / 360.0;
        if (abs(c.tgt_yaw - cur_yaw) > 0.25) {
          c.tgt_yaw = nearest_pmn(cur_yaw, c.tgt_yaw + 0.5);
          c.stride *= -1.0;
        }
        c.bend[IDX_L] = 0.0;
        c.bend[IDX_L] = 0.0;
        c.speed = globals::stdval::speed::normal;
        c.acclim = globals::stdval::acclim::standard;
        c.min_stepdur = 0;
        c.max_stepdur = globals::stdval::maxdur::safe;
        c.interval = 0;
        c.steps = -1;

        return;
      }

      if (201 <= idx && idx <= 213) {
        const auto finding_suid = idx - 200;
        if (b->cfg_.suid == finding_suid) {
          led_rs::finding_me = true;
        }

        m = b->cmd_.do_preset.prev_mode;
        b->cmd_.do_preset.idx = 0;

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
        //   b->cmd_.set_mags.strengths[i] = MagStren::Min;
        // }
        // for (uint8_t i = 0; i < 2; i++) {
        //   b->cmd_.set_mags.expected_state[i] = BOOL_RELEASE;
        // }
        // b->cmd_.set_mags.verif_thr = 1;
        // b->cmd_.set_mags.min_dur = 0;
        // b->cmd_.set_mags.max_dur = 100;
        // b->cmd_.set_mags.exit_to_mode = M::Idle_Init;
      }
    } break;
    default:
      break;
  }
}
