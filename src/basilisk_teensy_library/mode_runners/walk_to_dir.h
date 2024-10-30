#pragma once

#include "_meta.h"

void ModeRunners::WalkToDir() {
  switch (m) {
    case M::WalkToDir: {
      wd.init_yaw = b.rpl_.yaw();

      m = M::Walk;
      wa.c.init_didimbal = wd.c.init_didimbal;
      for (const auto f : IDX_LR) {
        wa.c.tgt_yaw[f] = [this] {
          wd.real_tgt_yaw = wd.c.tgt_yaw();
          if (isnan(wd.real_tgt_yaw)) {
            wd.real_tgt_yaw = wd.init_yaw;
          }
          const auto cur_yaw = b.rpl_.yaw();
          wd.real_tgt_yaw = nearest_pmn(cur_yaw, wd.real_tgt_yaw);

          if (wd.c.auto_moonwalk) {
            wd.moonwalk = (abs(wd.real_tgt_yaw - cur_yaw) > 0.25);
            if (wd.moonwalk) {
              wd.real_tgt_yaw = nearest_pmn(cur_yaw, wd.real_tgt_yaw + 0.5);
            }
          }

          return wd.real_tgt_yaw;
        };
        wa.c.tgt_yaw[f]();  // Call aforehead to ensure wd.moonwalk and
                            // wd.real_tgt_yaw update.
        wa.c.stride[f] = [this] {
          return wd.moonwalk ? -wd.c.stride() : wd.c.stride();
        };
        wa.c.bend[f] = wd.c.bend[f];
        wa.c.speed[f] = wd.c.speed;
        wa.c.acclim[f] = wd.c.acclim;
        wa.c.min_stepdur[f] = wd.c.min_stepdur;
        wa.c.max_stepdur[f] = wd.c.max_stepdur;
        wa.c.interval[f] = wd.c.interval;
      }
      wa.c.steps = wd.c.steps;
      wa.c.exit_condition = wd.c.exit_condition;
      wa.c.exit_to_mode = wd.c.exit_to_mode;
    } break;
    default:
      break;
  }
}
