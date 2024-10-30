#pragma once

#include "_meta.h"

void PPPShooter::WalkToPosInField(int last_4_digits) {
  // PPP WalkToPosInField range
  // Decimal 2ABCD
  // (AB) = tgt_pos_x
  //          == (AB) * 10cm
  // (CD) = tgt_pos_y
  //          == (CD) * 10cm
  uint8_t digits[4];
  for (uint8_t i = 0; i < 4; i++) {
    digits[i] = last_4_digits % 10;
    last_4_digits /= 10;
  }

  m = M::WalkToPosInField_Init;
  auto& c = b.cmd_.walk_to_pos_in_field;
  double x = (10 * digits[3] + digits[2]) * 10.0;
  double y = (10 * digits[1] + digits[0]) * 10.0;
  c.tgt_pos = Vec2{x, y};
}

void PPPShooter::WalkToDir(int last_3_digits) {
  // PPP WalkToDir range
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
    digits[i] = last_3_digits % 10;
    last_3_digits /= 10;
  }

  m = M::WalkToDir;
  auto& c = b.cmd_.walk_to_dir;
  c.init_didimbal = BOOL_L;
  c.auto_moonwalk = true;

  auto senw = digits[2];
  c.tgt_yaw = [this, senw] {
    return senw == 0 ? NaN
                     : nearest_pmn(b.imu_.GetYaw(true), (senw - 3) * 0.125);
  };

  const auto stride_intrep = digits[1];
  c.stride = [this, stride_intrep] {
    return stride_intrep == 0 ? 0.125 : stride_intrep / 36.0;
  };

  for (const auto f : IDX_LR) c.bend[f] = 0.0;
  c.speed = [] { return g::vars::speed; };
  c.acclim = [] { return g::c::acclim::standard; };
  c.min_stepdur = 0;
  c.max_stepdur = g::c::maxdur::safe;
  c.interval = 100;
  c.steps = digits[0] == 0 ? 10 : digits[0];
  c.exit_condition = [this] {
    return b.l_.failure_.stuck || b.r_.failure_.stuck;
  };
  c.exit_to_mode = M::Idle_Init;
}
