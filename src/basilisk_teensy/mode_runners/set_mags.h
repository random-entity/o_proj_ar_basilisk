#pragma once

#include "mode_runners.h"

void ModeRunners::SetMags(Basilisk* b) {
  auto& m = b->cmd_.mode;

  switch (m) {
    case M::SetMags: {
      auto& cmd = b->cmd_.set_mags;

      for (uint8_t id = 0; id < 4; id++) {
        b->mags_.SetStrength(id, cmd.strengths[id]);
      }

      b->cmd_.wait.init_time = millis();
      b->cmd_.wait.exit_condition = [](Basilisk* b) {
        if (millis() - b->cmd_.wait.init_time >
            b->cmd_.set_mags.max_wait_time) {
          return true;
        }

        for (uint8_t lr = 0; lr < 2; lr++) {
          if (b->cmd_.set_mags.expected_contact[lr]) {
            if (!b->lego_.state_[lr].Contact(
                    b->cmd_.set_mags.expected_consec_verif[lr])) {
              return false;
            }
          } else {
            if (!b->lego_.state_[lr].Detached(
                    b->cmd_.set_mags.expected_consec_verif[lr])) {
              return false;
            }
          }
        }

        return millis() - b->cmd_.wait.init_time >
               b->cmd_.set_mags.min_wait_time;
      };
      b->cmd_.wait.exit_to_mode = cmd.exit_mode;
      m = M::Wait;
    } break;
    default:
      break;
  }
}
