#pragma once

#include <elapsedMillis.h>

#include "_meta.h"

void ModeRunners::Wait() {
  switch (m) {
    case M::Wait: {
      if (wt.c.exit_condition()) m = wt.c.exit_to_mode;
    } break;
    default:
      break;
  }
}
