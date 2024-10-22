// Might NOT work with catwalk.

#pragma once

#include "_meta.h"

namespace walk_to_pos_in_field {
Vec2 force[13] = {Vec2{0.0, 0.0}, Vec2{0.0, 0.0}, Vec2{0.0, 0.0},
                  Vec2{0.0, 0.0}, Vec2{0.0, 0.0}, Vec2{0.0, 0.0},
                  Vec2{0.0, 0.0}, Vec2{0.0, 0.0}, Vec2{0.0, 0.0},
                  Vec2{0.0, 0.0}, Vec2{0.0, 0.0}, Vec2{0.0, 0.0},
                  Vec2{0.0, 0.0}};
Vec2 emergency_force = Vec2{0.0, 0.0};
bool trying_overlap_exit[13] = {false};
double cur_tgt_yaw;
bool moonwalk;
bool reinit;
}  // namespace walk_to_pos_in_field

void ModeRunners::WalkToPosInField(Basilisk* b) {
  auto& m = b->cmd_.mode;
  auto& c = b->cmd_.walk_to_pos_in_field;
  auto& w = b->cmd_.walk;

  switch (m) {
    case M::WalkToPosInField_Init: {
      walk_to_pos_in_field::reinit = false;
      const auto cur_yaw = b->imu_.GetYaw(true);
      walk_to_pos_in_field::cur_tgt_yaw =
          nearest_pmn(cur_yaw, (c.tgt_pos - b->lps_.GetPos()).arg());
      walk_to_pos_in_field::moonwalk =
          (abs(walk_to_pos_in_field::cur_tgt_yaw - cur_yaw) > 0.25);

      m = M::Walk;
      w.init_didimbal = BOOL_L;
      for (uint8_t f : IDX_LR) {
        w.tgt_yaw[f] = [](Basilisk* b) {
          const auto& c = b->cmd_.walk_to_pos_in_field;

          const auto pos = b->lps_.GetPos();
          const auto tgt_delta_pos = c.tgt_pos - pos;
          const auto pure_tgt_yaw_vec = tgt_delta_pos.normalize();

          auto field_tgt_yaw_vec = pure_tgt_yaw_vec;
          for (int i = 0; i < 13; i++) {
            field_tgt_yaw_vec =
                field_tgt_yaw_vec + walk_to_pos_in_field::force[i];
          }
          field_tgt_yaw_vec =
              field_tgt_yaw_vec + walk_to_pos_in_field::emergency_force;

          const auto field_tgt_yaw = field_tgt_yaw_vec.arg();

          walk_to_pos_in_field::cur_tgt_yaw = field_tgt_yaw;
          walk_to_pos_in_field::moonwalk =
              (abs(walk_to_pos_in_field::cur_tgt_yaw - b->imu_.GetYaw(true)) >
               0.25);

          return nearest_pmn(b->imu_.GetYaw(true),
                             walk_to_pos_in_field::moonwalk
                                 ? field_tgt_yaw + 0.5
                                 : field_tgt_yaw);
        };
        w.stride[f] = [](Basilisk* b) {
          double result = 0.125;
          return walk_to_pos_in_field::moonwalk ? -result : result;
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
        if (walk_to_pos_in_field::reinit) {
          // b->cmd_.pivseq.exit_to_mode = M::Wait;
          // b->cmd_.wait.init_time = millis();
          // b->cmd_.wait.exit_condition = [](Basilisk* b) {
          //   return millis() > b->cmd_.wait.init_time + 1000;
          // };
          // b->cmd_.wait.exit_to_mode = M::WalkToPosInField_Reinit;
          return true;
        }

        const auto& c = b->cmd_.walk_to_pos_in_field;

        if (b->lps_.GetPos().dist(c.tgt_pos) < 25.0) {
          b->cmd_.pivseq.exit_to_mode = M::Idle_Init;
          return true;
        }

        const auto pos = b->lps_.GetPos();
        const auto tgt_delta_pos = c.tgt_pos - pos;
        const auto pure_tgt_yaw = tgt_delta_pos.arg();
        const auto pure_tgt_yaw_vec = tgt_delta_pos.normalize();

        // Emergency reinit backward.
        if (b->Emergency()) {
          const auto cur_didimbal = b->cmd_.pivot.didimbal;
          if (cur_didimbal == BOOL_L) {
            b->mags_.SetStrength(2, MagStren::Min);
            b->mags_.SetStrength(3, MagStren::Min);
          } else {
            b->mags_.SetStrength(0, MagStren::Min);
            b->mags_.SetStrength(1, MagStren::Min);
          }
          b->CommandBoth([](Servo& s) { s->SetStop(); });

          walk_to_pos_in_field::emergency_force =
              1e6 * Vec2{walk_to_pos_in_field::cur_tgt_yaw + 0.5};
          walk_to_pos_in_field::reinit = true;
          b->cmd_.pivseq.exit_to_mode = M::Wait;
          b->cmd_.wait.init_time = millis();
          b->cmd_.wait.exit_condition = [](Basilisk* b) {
            return millis() >= b->cmd_.wait.init_time + 1000;
          };
          b->cmd_.wait.exit_to_mode = M::WalkToPosInField_Reinit;
          return true;
        }

        for (uint8_t other_suid = 1; other_suid <= 13; other_suid++) {
          if (other_suid == b->cfg_.suid) continue;

          const auto& other = roster::db[other_suid - 1];
          const auto other_pos = Vec2{other.x, other.y};

          const auto dist_vec = other_pos - pos;
          const auto dist = dist_vec.mag();

          // Consider all others regardless of distance.

          if (dist < b->overlap_thr_) {
            // Might be overlapping physically, and due to LPS error, at
            // front check is illegible. So just strongly push backwards.

            Pln("Overlap");

            if (!walk_to_pos_in_field::trying_overlap_exit[other_suid - 1]) {
              walk_to_pos_in_field::force[other_suid - 1] =
                  1e3 * Vec2{walk_to_pos_in_field::cur_tgt_yaw + 0.5};
              walk_to_pos_in_field::trying_overlap_exit[other_suid - 1] = true;
              walk_to_pos_in_field::reinit = true;
            }

            continue;
          } else {
            walk_to_pos_in_field::force[other_suid - 1] = Vec2{0.0, 0.0};
            walk_to_pos_in_field::trying_overlap_exit[other_suid - 1] = false;
          }

          // Not overlapping physically, so detour if the other is at front.

          const auto watch =
              nearest_pmn(0.0, dist_vec.argsub(pure_tgt_yaw_vec));

          const auto at_front = abs(watch) < 0.25;
          if (!at_front) continue;

          const auto at_right = watch < 0.0;

          const double r = max(dist - b->boundary_radius_, 1e-3);
          const double mag = 1.0 / sq(r);
          walk_to_pos_in_field::force[other_suid - 1] =
              mag * Vec2{dist_vec.arg() + (at_right ? 1.0 : -1.0) * 0.25};

          if (!b->lps_.BoundMinX())
            walk_to_pos_in_field::emergency_force =
                walk_to_pos_in_field::emergency_force + Vec2{0.0};
          else if (!b->lps_.BoundMaxX())
            walk_to_pos_in_field::emergency_force =
                walk_to_pos_in_field::emergency_force + Vec2{0.5};
          if (!b->lps_.BoundMinY())
            walk_to_pos_in_field::emergency_force =
                walk_to_pos_in_field::emergency_force + Vec2{0.25};
          else if (!b->lps_.BoundMaxY())
            walk_to_pos_in_field::emergency_force =
                walk_to_pos_in_field::emergency_force + Vec2{-0.25};
        }

        if (!walk_to_pos_in_field::reinit) {
          walk_to_pos_in_field::emergency_force = Vec2{0.0, 0.0};
        }

        // const auto cur_yaw = b->imu_.GetYaw(true);
        // const auto field_tgt_yaw_vec = pure_tgt_yaw_vec + force;
        // auto field_tgt_yaw = nearest_pmn(cur_yaw, field_tgt_yaw_vec.arg());
        // if (abs(field_tgt_yaw - cur_yaw) > 0.25) {
        //   walk_to_pos_in_field::moonwalk = true;
        //   field_tgt_yaw = nearest_pmn(cur_yaw, field_tgt_yaw + 0.5);
        // } else {
        //   walk_to_pos_in_field::moonwalk = false;
        // }
        // walk_to_pos_in_field::tgt_yaw = field_tgt_yaw;

        return walk_to_pos_in_field::reinit;
      };
      w.exit_condition(b);  // Call it once to update values.
      w.exit_to_mode = M::WalkToPosInField_Reinit;
    } break;
    case M::WalkToPosInField_Reinit: {
      walk_to_pos_in_field::reinit = false;

      const auto cur_yaw = b->imu_.GetYaw(true);
      walk_to_pos_in_field::cur_tgt_yaw =
          nearest_pmn(cur_yaw, walk_to_pos_in_field::cur_tgt_yaw);
      walk_to_pos_in_field::moonwalk =
          (abs(walk_to_pos_in_field::cur_tgt_yaw - cur_yaw) > 0.25);

      m = M::Walk;
      w.exit_to_mode = M::BounceWalk_Reinit;
    } break;
    default:
      break;
  }
}
