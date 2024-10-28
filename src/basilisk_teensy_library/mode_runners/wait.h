#pragma once

#include "_meta.h"

void ModeRunners::Wait() {
  switch (m) {
    case M::Wait: {
      if (c.wait.exit_condition()) m = c.wait.exit_to_mode;
    } break;
    default:
      break;
  }
}
