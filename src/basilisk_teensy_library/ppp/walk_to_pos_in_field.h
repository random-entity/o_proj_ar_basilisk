#pragma once

#include "_meta.h"

namespace ppp {
void Shooter::WalkToPosInField(int last_4_digits) {
  // PPP WalkToPosInField range
  // Decimal 1ABCD
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
}  // namespace ppp
