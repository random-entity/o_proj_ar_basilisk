#pragma once

#include "_meta.h"

void ModeRunners::Free() {
  switch (m) {
    case M::Free: {
      b.CommandBoth([](Servo& s) { s.SetStop(); });
      b.mags_.ReleaseAll();
      m = M::Wait;
      wt.c.since_init = 0;
      wt.c.exit_condition = [this] {
        return wt.c.since_init > g::c::maxdur::safe;
      };
      wt.c.exit_to_mode = M::Idle_Init;
    } break;
    default:
      break;
  }
}
