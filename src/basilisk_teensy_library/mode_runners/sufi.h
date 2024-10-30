#pragma once

#include "_meta.h"

void ModeRunners::Sufi() {
  switch (m) {
    case M::Sufi: {
      m = M::Walk;
      wa.c.init_didimbal = sf.c.init_didimbal;
      for (uint8_t f : IDX_LR) {
        wa.c.tgt_yaw[f] = [] { return NaN; };
        wa.c.bend[f] = sf.c.bend[f];
        wa.c.speed[f] = sf.c.speed;
        wa.c.acclim[f] = sf.c.acclim;
        wa.c.min_stepdur[f] = sf.c.min_stepdur;
        wa.c.max_stepdur[f] = sf.c.max_stepdur;
        wa.c.interval[f] = sf.c.interval;
      }
      wa.c.stride[IDX_L] = [this] { return sf.c.stride; };
      wa.c.stride[IDX_R] = [this] { return -sf.c.stride; };
      wa.c.steps = sf.c.steps;
      wa.c.exit_condition = [this] {
        return abs(b.rpl_.yaw() - sf.c.dest_yaw) < sf.c.exit_thr;
      };
      wa.c.exit_to_mode = M::Idle_Init;
    } break;
    default:
      break;
  }
}
