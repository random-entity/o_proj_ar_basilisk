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

      for (int mid = 0; mid < 4; mid++) {
        if (rm.dur[mid] == 0) {
          randomSeed(b.cfg_.suid * 100 + mid * millis());
          rm.dur[mid] = random(rm.c.min_phase_dur, rm.c.max_phase_dur);
        } else {
          const uint32_t since_switch = b.mags_.attaching_[mid]
                                            ? b.mags_.since_release_[mid]
                                            : b.mags_.since_attach_[mid];
          if (since_switch >= rm.dur[mid]) {
            b.mags_.SetStrength(mid, Bool2MS(!b.mags_.attaching_[mid]));
            rm.dur[mid] = 0;
          }
        }
      }
    } break;
    default:
      break;
  }
}
