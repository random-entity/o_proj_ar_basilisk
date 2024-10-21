#pragma once

#include "_meta.h"

namespace bounce_walk {
double tgt_yaw;
bool moonwalk;
bool reinit;
bool trying_overlap_exit[13] = {false};
}  // namespace bounce_walk

void ModeRunners::BounceWalk(Basilisk* b) {
  auto& m = b->cmd_.mode;
  auto& c = b->cmd_.bounce_walk;
  auto& w = b->cmd_.walk;

  switch (m) {
    case M::BounceWalk_Init: {
      bounce_walk::reinit = false;
      const auto cur_yaw = b->imu_.GetYaw(true);
      bounce_walk::tgt_yaw = nearest_pmn(cur_yaw, c.init_tgt_yaw);
      bounce_walk::moonwalk = (abs(bounce_walk::tgt_yaw - cur_yaw) > 0.25);

      m = M::Walk;
      w.init_didimbal = BOOL_L;
      for (LRIdx f : IDX_LR) {
        w.tgt_yaw[f] = [](Basilisk* b) {
          return bounce_walk::moonwalk
                     ? nearest_pmn(b->imu_.GetYaw(true),
                                   bounce_walk::tgt_yaw + 0.5)
                     : nearest_pmn(b->imu_.GetYaw(true), bounce_walk::tgt_yaw);
        };
        w.stride[f] = [](Basilisk*) {
          double result = 0.125;
          return bounce_walk::moonwalk ? -result : result;
        };
        w.bend[f] = 0.0;
        w.speed[f] = globals::stdval::speed::normal;
        w.acclim[f] = globals::stdval::acclim::standard;
        w.min_stepdur[f] = 0;
        w.max_stepdur[f] = globals::stdval::maxdur::safe;
        w.interval[f] = 100;
      }
      w.steps = -1;
      w.exit_condition = [](Basilisk* b) {
        if (bounce_walk::reinit) return true;

        // Emergency reinit backward.
        if (b->Emergency()) {
          Pln("Emergency");

          const auto cur_didimbal = b->cmd_.pivot.didimbal;
          if (cur_didimbal == BOOL_L) {
            b->mags_.SetStrength(2, MagStren::Min);
            b->mags_.SetStrength(3, MagStren::Min);
          } else {
            b->mags_.SetStrength(0, MagStren::Min);
            b->mags_.SetStrength(1, MagStren::Min);
          }
          b->CommandBoth([](Servo* s) { s->SetStop(); });

          bounce_walk::tgt_yaw = bounce_walk::tgt_yaw + 0.5;
          bounce_walk::reinit = true;
          b->cmd_.pivseq.exit_to_mode = M::Wait;
          b->cmd_.wait.init_time = millis();
          b->cmd_.wait.exit_condition = [](Basilisk* b) {
            return millis() >= b->cmd_.wait.init_time + 1000;
          };
          b->cmd_.wait.exit_to_mode = M::BounceWalk_Reinit;
          return true;
        }

        const auto my_pos = b->lps_.GetPos();
        const auto my_tgt_yaw = bounce_walk::tgt_yaw;

        Vec2 new_tgt_yaw = my_tgt_yaw;

        for (uint8_t other_suid = 1; other_suid <= 13; other_suid++) {
          if (other_suid == b->cfg_.suid) continue;

          const auto& other = roster::db[other_suid - 1];
          const auto other_pos = Vec2{other.x, other.y};

          if (other_pos.isnan()) continue;

          const auto dist_vec = other_pos - my_pos;

          if (dist_vec.mag() > b->boundary_radius_) continue;

          // At this point, the boundaries have collided.
          P("Collision with ");
          Serial.println(other_suid);

          if (dist_vec.mag() < b->overlap_thr_) {
            // Might be overlapping physically, and due to LPS error,
            // at-front check is illegible.

            Pln("Overlap");

            if (!bounce_walk::trying_overlap_exit[other_suid - 1]) {
              new_tgt_yaw = new_tgt_yaw + 1e6 * Vec2{my_tgt_yaw + 0.5};
              bounce_walk::trying_overlap_exit[other_suid - 1] = true;
              bounce_walk::reinit = true;
            }

            continue;
          } else {
            bounce_walk::trying_overlap_exit[other_suid - 1] = false;
          }

          // Not overlapping physically, so bounce if the other is at front.

          const auto at_front =
              abs(nearest_pmn(0.0, dist_vec.arg() - my_tgt_yaw)) < 0.25;
          if (!at_front) continue;

          Pln("At front");

          new_tgt_yaw = new_tgt_yaw + Vec2{dist_vec.arg() + 0.5};
        }

        bounce_walk::tgt_yaw = new_tgt_yaw.arg();

        if ((!b->lps_.BoundMinX() &&
             abs(0.5 - nearest_pmn(0.5, bounce_walk::tgt_yaw)) <= 0.25) ||
            (!b->lps_.BoundMaxX() &&
             abs(nearest_pmn(0.0, bounce_walk::tgt_yaw)) <= 0.25)) {
          bounce_walk::tgt_yaw = 0.5 - bounce_walk::tgt_yaw;
          bounce_walk::reinit = true;
        }

        if ((!b->lps_.BoundMinY() &&
             abs(-0.25 - nearest_pmn(-0.25, bounce_walk::tgt_yaw)) <= 0.25) ||
            (!b->lps_.BoundMaxY() &&
             abs(0.25 - nearest_pmn(0.25, bounce_walk::tgt_yaw)) <= 0.25)) {
          bounce_walk::tgt_yaw *= -1.0;
          bounce_walk::reinit = true;
        }

        return bounce_walk::reinit;
      };
      w.exit_to_mode = M::BounceWalk_Reinit;
    } break;
    case M::BounceWalk_Reinit: {
      bounce_walk::reinit = false;
      const auto cur_yaw = b->imu_.GetYaw(true);
      bounce_walk::tgt_yaw = nearest_pmn(cur_yaw, bounce_walk::tgt_yaw);
      bounce_walk::moonwalk = (abs(bounce_walk::tgt_yaw - cur_yaw) > 0.25);

      m = M::Walk;
      w.exit_to_mode = M::BounceWalk_Reinit;
    } break;
    default:
      break;
  }
}
