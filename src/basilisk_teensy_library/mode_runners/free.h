#pragma once

#include "_meta.h"

void ModeRunners::Free() {
  switch (m) {
    case M::Free: {
      b.CommandBoth([](Servo& s) { s.SetStop(); });
      b.mags_.ReleaseAll();
      m = M::Wait;
      c.wait.since_init = 0;
      c.wait.exit_condition = [this] {
        return c.wait.since_init > g::c::maxdur::safe;
      };
      c.wait.exit_to_mode = M::Idle_Init;
    } break;
    default:
      break;
  }
}
