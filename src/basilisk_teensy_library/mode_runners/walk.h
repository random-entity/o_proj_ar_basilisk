#pragma once

#include "_meta.h"

void ModeRunners::Walk() {
  switch (m) {
    case M::Walk: {
      m = M::PivSeq_Init;
      ps.c.pivots = [this](uint32_t step) {
        Basilisk::Command::Pivot p;
        p.didimbal = step % 2 == 0 ? wa.c.init_didimbal : !wa.c.init_didimbal;
        const auto didim_idx = p.didimbal == BOOL_L ? IDX_L : IDX_R;
        const auto kick_idx = p.didimbal == BOOL_L ? IDX_R : IDX_L;
        p.tgt_yaw = wa.c.tgt_yaw[didim_idx];
        p.stride = wa.c.stride[didim_idx];
        if (step == 0) {
          p.bend[didim_idx] = wa.c.bend[didim_idx];
        } else {
          p.bend[didim_idx] = NaN;
        }
        p.bend[kick_idx] = wa.c.bend[kick_idx];
        p.speed = wa.c.speed[didim_idx];
        p.acclim = wa.c.acclim[didim_idx];
        p.min_dur = wa.c.min_stepdur[didim_idx];
        p.max_dur = wa.c.max_stepdur[didim_idx];
        p.exit_condition = wa.c.exit_condition;
        return p;
      };
      ps.c.intervals = [this](uint32_t step) {
        const LR didimbal =
            step % 2 == 0 ? wa.c.init_didimbal : !wa.c.init_didimbal;
        const auto didim_idx = didimbal == BOOL_L ? IDX_L : IDX_R;
        return wa.c.interval[didim_idx];
      };
      ps.c.steps = wa.c.steps;
      ps.c.exit_condition = wa.c.exit_condition;
      ps.c.exit_to_mode = wa.c.exit_to_mode;
    } break;
    default:
      break;
  }
}
