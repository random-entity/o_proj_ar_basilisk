#pragma once

#include "_meta.h"

void ModeRunners::PivSeq() {
  switch (m) {
    case M::PivSeq_Init: {
      ps.cur_step = 0;
      m = M::PivSeq_Step;
    } break;
    case M::PivSeq_Step: {
      if (ps.c.exit_condition() ||
          (ps.c.steps >= 0 &&
           ps.cur_step >= static_cast<uint32_t>(ps.c.steps))) {
        m = ps.c.exit_to_mode;
        return;
      }

      m = M::Pivot_Init;
      pv.c = ps.c.pivots(ps.cur_step);
      pv.c.exit_to_mode = M::Wait;
      wt.c.since_init = 0;
      wt.c.exit_condition = [this] {
        return wt.c.since_init >= ps.c.intervals(ps.cur_step);
      };
      wt.c.exit_to_mode = M::PivSeq_Step;

      ps.cur_step++;
    } break;
    default:
      break;
  }
}
