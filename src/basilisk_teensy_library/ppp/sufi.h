#pragma once

#include "_meta.h"

namespace ppp {

void Shooter::Sufi(uint16_t idx) {
  // PPP Sufi range
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
  auto& c = b.cmd_.sufi;
  c.init_didimbal = BOOL_L;
  c.dest_yaw = digits[1] == 0
                   ? NaN
                   : nearest_pmn(b.imu_.GetYaw(true), (digits[1] - 3) * 0.125);
  c.exit_thr = 0.01;
  c.stride = digits[0] == 0 ? 0.125 : digits[0] / 36.0;
  if (!isnan(c.dest_yaw)) {
    bool dest_is_greater = c.dest_yaw > b.imu_.GetYaw(true);
    if (!dest_is_greater) {
      c.stride *= -1.0;
    }
  }
  for (const auto f : IDX_LR) c.bend[f] = 0.0;
  c.speed = [] { return g::vars::speed; };
  c.acclim = [] { return g::c::acclim::standard; };
  c.min_stepdur = 0;
  c.max_stepdur = g::c::maxdur::safe;
  c.interval = 100;
  c.steps = -1;
}

}  // namespace ppp
