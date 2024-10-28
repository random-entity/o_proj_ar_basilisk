#pragma once

#include "_meta.h"

void ModeRunners::WalkToDir() {
  switch (m) {
    case M::WalkToDir: {
      wd.init_yaw = b.yaw();

      m = M::Walk;
      wa.c.init_didimbal = wd.c.init_didimbal;

      for (const auto f : IDX_LR) {
        wa.c.tgt_yaw[f] = [this] {
          auto tgt_yaw = wd.c.tgt_yaw();
          if (isnan(tgt_yaw)) tgt_yaw = wd.init_yaw;
          return tgt_yaw;
        };
        wa.c.stride[f] = wd.c.stride;
        wa.c.bend[f] = wd.c.bend[f];
        wa.c.speed[f] = wd.c.speed;
        wa.c.acclim[f] = wd.c.acclim;
        wa.c.min_stepdur[f] = wd.c.min_stepdur;
        wa.c.max_stepdur[f] = wd.c.max_stepdur;
        wa.c.interval[f] = wd.c.interval;
      }
      wa.c.steps = wd.c.steps;
      wa.c.exit_condition = [this] {
        return !(b.lps_.Bound()) || !!(b.BoundaryCollision());
      };
      wa.c.exit_to_mode = M::Idle_Init;
    } break;
    default:
      break;
  }
}
