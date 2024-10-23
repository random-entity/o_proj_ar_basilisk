#pragma once

#include "_meta.h"

void ModeRunners::RandomMags(Basilisk* b) {
  static uint32_t init_time;
  static uint32_t dur[4] = {0, 0, 0, 0};

  auto& m = b->cmd_.mode;
  auto& c = b->cmd_.random_mags;

  switch (m) {
    case M::RandomMags_Init: {
      init_time = millis();
      b->CommandBoth([](Servo& s) { s->SetStop(); });
      m = M::RandomMags_Do;
    } break;
    case M::RandomMags_Do: {
      if (millis() - init_time > b->cmd_.random_mags.dur) {
        m = M::Idle_Init;
        return;
      }

      for (uint8_t id = 0; id < 4; id++) {
        if (dur[id] == 0) {
          randomSeed(b->cfg_.suid * 100 + id * millis());
          dur[id] = random(c.min_phase_dur, c.max_phase_dur);
        } else {
          const uint32_t time_since_last_switch =
              b->mags_.attaching_[id] ? b->mags_.since_release_[id]
                                      : b->mags_.since_attach_[id];
          if (time_since_last_switch >= dur[id]) {
            b->mags_.SetStrength(id, Bool2MS(!b->mags_.attaching_[id]));
            dur[id] = 0;
          }
        }
      }
    } break;
    default:
      break;
  }
}