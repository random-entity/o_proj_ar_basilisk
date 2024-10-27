#pragma once

#include "_meta.h"

void ModeRunners::Wait() {
  static auto& m = b.cmd_.mode;
  static auto& c = b.cmd_.wait;

  switch (m) {
    case M::Wait: {
      if (c.exit_condition()) m = c.exit_to_mode;
    } break;
    default:
      break;
  }
}
