#pragma once

#include <Arduino.h>

#include "using_moteus.h"

double GetExtraValue(const QRpl& rpl, const uint16_t reg) {
  for (int i = 0; i < 8; i++) {
    if (rpl.extra[i].register_number == reg) {
      return rpl.extra[i].value;
    }
  }
  return NaN;
}
