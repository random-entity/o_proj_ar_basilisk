#pragma once

#include "_meta.h"

namespace diamond {
double init_yaw;
}  // namespace diamond

void ModeRunners::Diamond(Basilisk* b) {
  auto& m = b->cmd_.mode;
  auto& c = b->cmd_.diamond;
  auto& ps = b->cmd_.pivseq;

  switch (m) {
    case M::Diamond: {
      diamond::init_yaw = b->imu_.GetYaw(true);
      c.init_stride = nearest_pmn(0.0, c.init_stride);

      m = M::PivSeq_Init;
      ps.pivots = [](Basilisk* b, int idx) {
        auto& c = b->cmd_.diamond;
        Basilisk::Command::Pivot p;
        p.didimbal = idx % 2 == 0 ? c.init_didimbal : !c.init_didimbal;
        p.tgt_yaw = [](Basilisk*) { return diamond::init_yaw; };
        p.stride = idx % 4 <= 1         ? c.init_stride
                   : c.init_stride >= 0 ? c.init_stride - 0.5
                                        : c.init_stride + 0.5;
        p.bend[IDX_L] = 0.0;
        p.bend[IDX_R] = 0.0;
        p.speed = c.speed;
        p.acclim = c.acclim;
        p.min_dur = c.min_stepdur;
        p.max_dur = c.max_stepdur;
        p.exit_condition = nullptr;
        return p;
      };
      ps.intervals = [](Basilisk* b, int idx) {
        auto& c = b->cmd_.diamond;
        return c.interval;
      };
      ps.steps = c.steps;
      ps.exit_condition = nullptr;
      ps.exit_to_mode = M::Idle_Init;
    } break;
    default:
      break;
  }
}
