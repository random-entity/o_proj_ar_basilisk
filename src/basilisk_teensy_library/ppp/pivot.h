#pragma once

#include "_meta.h"

namespace ppp {

namespace pivot {
double tgt_yaw;
}  // namespace pivot

void Shooter::Pivot(uint16_t idx) {
  // PPP Pivot range
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
  auto& c = b.cmd_.pivot;
  c.didimbal = digits[3] == 1 ? BOOL_L : BOOL_R;
  ppp::pivot::tgt_yaw = (digits[2] - 3) * 0.125;
  c.tgt_yaw = [this] {
    const auto cur_yaw = b.imu_.GetYaw(true);
    auto ty = nearest_pmn(cur_yaw, ppp::pivot::tgt_yaw);
    if (b.cmd_.pivot.didimbal == BOOL_L && ty - cur_yaw < -(0.375 + 0.0625)) {
      ty += 1.0;
    } else if (b.cmd_.pivot.didimbal == BOOL_R &&
               ty - cur_yaw > 0.375 + 0.0625) {
      ty -= 1.0;
    }
    return ty;
  };
  c.stride = [] { return 0.0; };
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
  c.speed = [] { return g::vars::speed; };
  c.acclim = [] { return g::c::acclim::standard; };
  c.min_dur = 0;
  c.max_dur = g::c::maxdur::safe;
  c.exit_condition = [] { return false; };
  c.exit_to_mode = M::Idle_Init;

  return;
}

}  // namespace ppp
