#pragma once

#include <elapsedMillis.h>

#include "_meta.h"

void ModeRunners::SetMags() {
  static elapsedMillis since_init;

  switch (m) {
    case M::SetMags_Init: {
      b.CommandBoth([](Servo& s) { s.SetStop(); });
      for (int id = 0; id < 4; id++) {
        b.mags_.SetStrength(id, c.set_mags.strengths[id]);
      }
      since_init = 0;
      m = M::SetMags_Wait;
    } break;
    case M::SetMags_Wait: {
      if ([&] {
            if (since_init > c.set_mags.max_dur) {
              return true;
            }
            if (since_init < c.set_mags.min_dur) {
              return false;
            }
            for (const auto f : IDX_LR) {
              if (c.set_mags.expected_state[f]) {
                if (!b.lego_.state_[f].ConsecutiveContact(c.set_mags.verif_thr)) {
                  return false;
                }
              } else {
                if (!b.lego_.state_[f].ConsecutiveDetachment(c.set_mags.verif_thr)) {
                  return false;
                }
              }
            }
            return true;
          }()) {
        m = c.set_mags.exit_to_mode;
      };
    } break;
    default:
      break;
  }
}
