#pragma once

#include "_meta.h"

namespace ppp {

namespace piv_spin {
PhiSpeed speed = g::c::speed::normal;
}  // namespace piv_spin

void PPP::PivSpin(uint16_t idx) {
  // PPP PivSpin range
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
  auto& c = b.cmd_.piv_spin;
  c.didimbal = digits[2] == 1 ? BOOL_L : BOOL_R;
  c.dest_yaw = digits[1] == 0
                   ? NaN
                   : nearest_pmn(b.imu_.GetYaw(true), (digits[1] - 3) * 0.125);
  c.exit_thr = 0.01;
  c.stride = digits[0] == 0 ? 0.125 : digits[0] / 36.0;
  if (!isnan(c.dest_yaw)) {
    bool dest_is_greater = c.dest_yaw > b.imu_.GetYaw(true);
    bool positive_stride_drives_greater = c.didimbal == BOOL_L;
    if (dest_is_greater != positive_stride_drives_greater) {
      c.stride *= -1.0;
    }
  }
  for (uint8_t f : IDX_LR) c.bend[f] = 0.0;
  c.speed = [] { return ppp::piv_spin::speed; };
  c.acclim = [] { return g::c::acclim::standard; };
  c.min_stepdur = 0;
  c.max_stepdur = g::c::maxdur::safe;
  c.interval = 0;
  c.steps = -1;
}

}  // namespace ppp
