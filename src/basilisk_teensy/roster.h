#pragma once

#include <Arduino.h>
#include <elapsedMillis.h>

#ifndef NaN
#define NaN (0.0 / 0.0)
#endif

struct {
  double x = NaN, y = NaN;
  double yaw = NaN;
  elapsedMicros time_since_update_us = 0;
} roster[13];
