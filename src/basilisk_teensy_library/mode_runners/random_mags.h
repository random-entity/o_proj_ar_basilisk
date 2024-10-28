#pragma once

#include <elapsedMillis.h>

#include "_meta.h"

void ModeRunners::RandomMags() {
  switch (m) {
    case M::RandomMags_Init: {
      rm.since_init = 0;
      b.CommandBoth([](Servo& s) { s.SetStop(); });
      m = M::RandomMags_Do;
    } break;
    case M::RandomMags_Do: {
      if (rm.since_init > rm.c.dur) {
        m = M::Idle_Init;
        return;
      }

      for (int id = 0; id < 4; id++) {
        if (rm.dur[id] == 0) {
          randomSeed(b.cfg_.suid * 100 + id * millis());
          rm.dur[id] = random(rm.c.min_phase_dur, rm.c.max_phase_dur);
        } else {
          const uint32_t since_switch = b.mags_.attaching_[id]
                                            ? b.mags_.since_release_[id]
                                            : b.mags_.since_attach_[id];
          if (since_switch >= rm.dur[id]) {
            b.mags_.SetStrength(id, Bool2MS(!b.mags_.attaching_[id]));
            rm.dur[id] = 0;
          }
        }
      }
    } break;
    default:
      break;
  }
}
