#pragma once

#include "_meta.h"

void ModeRunners::BounceWalk() {
  switch (m) {
    case M::BounceWalk_Init: {
      bw.reinit = false;
      for (int suidm1 = 0; suidm1 < 13; suidm1++) {
        bw.trying_overlap_exit[suidm1] = false;
      }

      m = M::WalkToDir;
      wd.c.init_didimbal = BOOL_L;
      wd.c.auto_moonwalk = true;
      wd.c.tgt_yaw = [this] { return bw.c.init_tgt_yaw; };
      wd.c.stride = [] { return 0.125; };
      for (const auto f : IDX_LR) {
        wd.c.bend[f] = 0.0;
      }
      wd.c.speed = [] { return g::c::speed::normal; };
      wd.c.acclim = [] { return g::c::acclim::standard; };
      wd.c.min_stepdur = 0;
      wd.c.max_stepdur = g::c::maxdur::safe;
      wd.c.interval = 100;
      wd.c.steps = -1;
      wd.c.exit_condition = [this] {
        if (bw.reinit) return true;

        const auto cur_tgt_yaw = wd.c.tgt_yaw();

        if (b.l_.failure_.stuck || b.r_.failure_.stuck) {
#if DEBUG_FAILURE
          Pln("BounceWalk: Stuck");
#endif
          // Release kickbal and Stop.
          const auto cur_didimbal = pv.c.didimbal;
          if (cur_didimbal == BOOL_L) {
            b.mags_.SetStrength(2, MagStren::Min);
            b.mags_.SetStrength(3, MagStren::Min);
          } else {
            b.mags_.SetStrength(0, MagStren::Min);
            b.mags_.SetStrength(1, MagStren::Min);
          }
          b.CommandBoth([](Servo& s) { s.SetStop(); });

          // Reinit.
          ps.c.exit_to_mode = M::Wait;
          wt.c.since_init = 0;
          wt.c.exit_condition = [this]() { return wt.c.since_init >= 1000; };
          wt.c.exit_to_mode = M::BounceWalk_Reinit;
          wd.c.tgt_yaw = [=] { return cur_tgt_yaw + 0.5; };
          bw.reinit = true;
          return true;
        }

        const auto cur_pos = b.lps_.GetPos();
        Vec2 maybe_new_tgt_yaw_vec{0.0, 0.0};

        for (int other_suidm1 = 0; other_suidm1 < 13; other_suidm1++) {
          if (other_suidm1 == b.cfg_.suidm1) continue;

          const auto& other = roster[other_suidm1];
          const auto other_pos = Vec2{other.x, other.y};

          if (other_pos.isnan() || other.since_update_us > 3e6) {
#if DEBUG_FAILURE
            P("Roster not updated > 3s -> ");
            Serial.println(other_suidm1 + 1);
#endif
            continue;
          }

          const auto dist_vec = other_pos - cur_pos;

          if (dist_vec.mag() > b.cfg_.boundary_collision_thr) {
            // No problem.
            bw.trying_overlap_exit[other_suidm1] = false;
            continue;
          }

          // At this point, the boundaries have collided.
#if DEBUG_FAILURE
          P("Boundary collision with ");
          Serial.println(other_suidm1 + 1);
#endif

          if (dist_vec.mag() < b.cfg_.overlapping_collision_thr) {
            // This might not work well. Rather delegate overlap exit to
            // Servo-stuck handling logic.
            /*
            // Might be overlapping physically. Due to LPS error, at-front check
            // is illegible.
#if DEBUG_FAILURE
            P("Overlapping collision with ");
            Serial.println(other_suidm1 + 1);
#endif
            if (bw.trying_overlap_exit[other_suidm1]) {
#if DEBUG_FAILURE
              P("Trying overlap exit from ");
              Serial.println(other_suidm1 + 1);
#endif
            } else {
              maybe_new_tgt_yaw_vec += 1e6 * Vec2{cur_tgt_yaw + 0.5};
              bw.trying_overlap_exit[other_suidm1] = true;
              bw.reinit = true;
            }
            */
            continue;
          }

          bw.trying_overlap_exit[other_suidm1] = false;

          // At this point, the boundaries have collided, but they are not
          // overlapping, so hopefully at-front check is legible.
          // So bounce if the other is at front.

          const auto at_front =
              abs(nearest_pmn(0.0, dist_vec.arg() - cur_tgt_yaw)) < 0.25;
          if (!at_front) continue;

#if DEBUG_FAILURE
          P("At front -> ");
          Serial.println(other_suidm1 + 1);
#endif

          maybe_new_tgt_yaw_vec += Vec2{dist_vec.arg() + 0.5};
          bw.reinit = true;
        }

        double maybe_new_tgt_yaw =
            bw.reinit ? maybe_new_tgt_yaw_vec.arg() : cur_tgt_yaw;

        if ((!b.lps_.BoundMinX() &&
             abs(0.5 - nearest_pmn(0.5, maybe_new_tgt_yaw)) < 0.25) ||
            (!b.lps_.BoundMaxX() &&
             abs(nearest_pmn(0.0, maybe_new_tgt_yaw)) < 0.25)) {
          maybe_new_tgt_yaw = 0.5 - maybe_new_tgt_yaw;
          bw.reinit = true;
        }

        if ((!b.lps_.BoundMinY() &&
             abs(-0.25 - nearest_pmn(-0.25, maybe_new_tgt_yaw)) < 0.25) ||
            (!b.lps_.BoundMaxY() &&
             abs(0.25 - nearest_pmn(0.25, maybe_new_tgt_yaw)) < 0.25)) {
          maybe_new_tgt_yaw *= -1.0;
          bw.reinit = true;
        }

        if (bw.reinit) {
          wd.c.tgt_yaw = [=] { return maybe_new_tgt_yaw; };
        }

        return bw.reinit;
      };
      wd.c.exit_to_mode = M::BounceWalk_Reinit;
    } break;
    case M::BounceWalk_Reinit: {
      bw.reinit = false;
      m = M::WalkToDir;
      wd.c.exit_to_mode = M::BounceWalk_Reinit;
    } break;
    default:
      break;
  }
}
