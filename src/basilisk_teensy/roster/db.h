#pragma once

#include <Arduino.h>

#ifndef NaN
#define NaN (0.0 / 0.0)
#endif

struct PosYaw {
  double x = NaN, y = NaN;
  double yaw = NaN;
};

namespace roster {
PosYaw db[13];
uint32_t updated_time[13];
}  // namespace roster
