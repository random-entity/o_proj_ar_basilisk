#pragma once

#include "_meta.h"

void ModeRunners::PivSpin() {
  switch (m) {
    case M::PivSpin: {
      m = M::PivSeq_Init;
      ps.c.pivots = [this](int) {
        Basilisk::Command::Pivot p;
        p.didimbal = sp.c.didimbal;
        p.tgt_yaw = [] { return NaN; };
        p.stride = [this] { return sp.c.stride; };
        for (const auto f : IDX_LR) p.bend[f] = sp.c.bend[f];
        p.speed = sp.c.speed;
        p.acclim = sp.c.acclim;
        p.min_dur = sp.c.min_stepdur;
        p.max_dur = sp.c.max_stepdur;
        p.exit_condition = [this] {
          return abs(b.yaw() - sp.c.dest_yaw) < sp.c.exit_thr;
        };
        return p;
      };
      ps.c.intervals = [this](int) { return sp.c.interval; };
      ps.c.steps = sp.c.steps;
      ps.c.exit_condition = [this] {
        return abs(b.yaw() - sp.c.dest_yaw) < sp.c.exit_thr;
      };
      ps.c.exit_to_mode = M::Idle_Init;
    } break;
    default:
      break;
  }
}
