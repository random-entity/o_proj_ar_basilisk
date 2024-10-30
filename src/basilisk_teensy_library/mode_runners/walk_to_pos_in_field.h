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

void ModeRunners::WalkToPosInField() {
  switch (m) {
    case M::WalkToPosInField_Init: {
      m = M::WalkToDir;
      wd.c.init_didimbal = BOOL_L;
      wd.c.auto_moonwalk = true;
      wd.c.tgt_yaw = [this] {
        
      };
      wd.c.stride = [] { return 0.125; };

      /////////////////////////////////////////////
      /////////////////////////////////////////////
      /////////////////////////////////////////////
      /////////////////////////////////////////////

      wf.reinit = false;
      const auto cur_yaw = b.rpl_.yaw();
      wf.cur_tgt_yaw =
          nearest_pmn(cur_yaw, (wf.c.tgt_pos - b->lps_.GetPos()).arg());

      //////////////////////////////////////////////////////////

      wf.moonwalk = (abs(wf.cur_tgt_yaw - cur_yaw) > 0.25);

      m = M::Walk;
      wa.c.init_didimbal = BOOL_L;
      for (uint8_t f : IDX_LR) {
        wa.c.tgt_yaw[f] = [](Basilisk* b) {
          const auto& wf.c = b->cmd_.walk_to_pos_in_field;

          const auto pos = b->lps_.GetPos();
          const auto tgt_delta_pos = wf.c.tgt_pos - pos;
          const auto pure_tgt_yaw_vec = tgt_delta_pos.normalize();

          auto field_tgt_yaw_vec = pure_tgt_yaw_vec;
          for (int i = 0; i < 13; i++) {
            field_tgt_yaw_vec = field_tgt_yaw_vec + wf.force[i];
          }
          field_tgt_yaw_vec = field_tgt_yaw_vec + wf.emergency_force;

          const auto field_tgt_yaw = field_tgt_yaw_vec.arg();

          wf.cur_tgt_yaw = field_tgt_yaw;
          wf.moonwalk = (abs(wf.cur_tgt_yaw - b.rpl_.yaw()) > 0.25);

          return nearest_pmn(b.rpl_.yaw(),
                             wf.moonwalk ? field_tgt_yaw + 0.5 : field_tgt_yaw);
        };
        wa.c.stride[f] = [](Basilisk* b) {
          double result = 0.125;
          return wf.moonwalk ? -result : result;
        };
        wa.c.bend[f] = 0.0;
        wa.c.speed[f] = globals::stdval::speed::normal;
        wa.c.acclim[f] = globals::stdval::acclim::standard;
        wa.c.min_stepdur[f] = 0;
        wa.c.max_stepdur[f] = globals::stdval::maxdur::safe;
        wa.c.interval[f] = 100;
      }
      wa.c.steps = -1;
      wa.c.exit_condition = [](Basilisk* b) {
        if (wf.reinit) {
          // b->cmd_.pivseq.exit_to_mode = M::Wait;
          // b->cmd_.wait.init_time = millis();
          // b->cmd_.wait.exit_condition = [](Basilisk* b) {
          //   return millis() > b->cmd_.wait.init_time + 1000;
          // };
          // b->cmd_.wait.exit_to_mode = M::WalkToPosInField_Reinit;
          return true;
        }

        const auto& wf.c = b->cmd_.walk_to_pos_in_field;

        if (b->lps_.GetPos().dist(wf.c.tgt_pos) < 25.0) {
          b->cmd_.pivseq.exit_to_mode = M::Idle_Init;
          return true;
        }

        const auto pos = b->lps_.GetPos();
        const auto tgt_delta_pos = wf.c.tgt_pos - pos;
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

          wf.emergency_force = 1e6 * Vec2{wf.cur_tgt_yaw + 0.5};
          wf.reinit = true;
          b->cmd_.pivseq.exit_to_mode = M::Wait;
          b->cmd_.wait.since_init = millis();
          b->cmd_.wait.exit_condition = [](Basilisk* b) {
            return millis() >= b->cmd_.wait.since_init + 1000;
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

            if (!wf.trying_overlap_exit[other_suid - 1]) {
              wf.force[other_suid - 1] = 1e3 * Vec2{wf.cur_tgt_yaw + 0.5};
              wf.trying_overlap_exit[other_suid - 1] = true;
              wf.reinit = true;
            }

            continue;
          } else {
            wf.force[other_suid - 1] = Vec2{0.0, 0.0};
            wf.trying_overlap_exit[other_suid - 1] = false;
          }

          // Not overlapping physically, so detour if the other is at front.

          const auto watch =
              nearest_pmn(0.0, dist_vec.argsub(pure_tgt_yaw_vec));

          const auto at_front = abs(watch) < 0.25;
          if (!at_front) continue;

          const auto at_right = watch < 0.0;

          const double r = max(dist - b->boundary_radius_, 1e-3);
          const double mag = 1.0 / sq(r);
          wf.force[other_suid - 1] =
              mag * Vec2{dist_vec.arg() + (at_right ? 1.0 : -1.0) * 0.25};

          if (!b->lps_.BoundMinX())
            wf.emergency_force = wf.emergency_force + Vec2{0.0};
          else if (!b->lps_.BoundMaxX())
            wf.emergency_force = wf.emergency_force + Vec2{0.5};
          if (!b->lps_.BoundMinY())
            wf.emergency_force = wf.emergency_force + Vec2{0.25};
          else if (!b->lps_.BoundMaxY())
            wf.emergency_force = wf.emergency_force + Vec2{-0.25};
        }

        if (!wf.reinit) {
          wf.emergency_force = Vec2{0.0, 0.0};
        }

        // const auto cur_yaw = b.rpl_.yaw();
        // const auto field_tgt_yaw_vec = pure_tgt_yaw_vec + force;
        // auto field_tgt_yaw = nearest_pmn(cur_yaw, field_tgt_yaw_vec.arg());
        // if (abs(field_tgt_yaw - cur_yaw) > 0.25) {
        //   wf.moonwalk = true;
        //   field_tgt_yaw = nearest_pmn(cur_yaw, field_tgt_yaw + 0.5);
        // } else {
        //   wf.moonwalk = false;
        // }
        // wf.tgt_yaw = field_tgt_yaw;

        return wf.reinit;
      };
      wa.c.exit_condition(b);  // Call it once to update values.
      wa.c.exit_to_mode = M::WalkToPosInField_Reinit;
    } break;
    case M::WalkToPosInField_Reinit: {
      wf.reinit = false;

      const auto cur_yaw = b.rpl_.yaw();

      wf.cur_tgt_yaw = nearest_pmn(cur_yaw, wf.cur_tgt_yaw);
      wf.moonwalk = (abs(wf.cur_tgt_yaw - cur_yaw) > 0.25);

      m = M::Walk;
      wa.c.exit_to_mode = M::BounceWalk_Reinit;
    } break;
    default:
      break;
  }
}
