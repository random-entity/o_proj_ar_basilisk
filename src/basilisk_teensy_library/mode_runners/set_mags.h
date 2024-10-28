#pragma once

#include <elapsedMillis.h>

#include "_meta.h"

void ModeRunners::SetMags() {
  switch (m) {
    case M::SetMags_Init: {
      b.CommandBoth([](Servo& s) { s.SetStop(); });
      for (int id = 0; id < 4; id++) {
        b.mags_.SetStrength(id, mg.c.strengths[id]);
      }
      mg.since_init = 0;
      m = M::SetMags_Wait;
    } break;
    case M::SetMags_Wait: {
      if ([&] {
            if (mg.since_init > mg.c.max_dur) {
              return true;
            }
            if (mg.since_init < mg.c.min_dur) {
              return false;
            }
            for (const auto f : IDX_LR) {
              if (mg.c.expected_state[f]) {
                if (!b.lego_.state_[f].ConsecutiveContact(mg.c.verif_thr)) {
                  return false;
                }
              } else {
                if (!b.lego_.state_[f].ConsecutiveDetachment(mg.c.verif_thr)) {
                  return false;
                }
              }
            }
            return true;
          }()) {
        m = mg.c.exit_to_mode;
      };
    } break;
    default:
      break;
  }
}
