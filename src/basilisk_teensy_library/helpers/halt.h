#pragma once

#include <Arduino.h>

#include "../globals/serials.h"

void HALT(const char* err_str = nullptr) {
#if ENABLE_SERIAL
  Pln("x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x");
  if (err_str) Pln(err_str);
  Pln("Halting program");
#endif

  while (1);
}
