#pragma once

#include "_meta.h"

void ModeRunners::WalkToPosInField() {
  switch (m) {
    case M::WalkToPosInField_Init: {
      wf.reinit = false;

      m = M::WalkToDir;
      wd.c.init_didimbal = pv.c.didimbal;
      wd.c.auto_moonwalk = true;
      wd.c.tgt_yaw = [this] {
        auto pos = b.lps_.GetPos();
        Vec2 tgt_delta_pos_n = wf.c.tgt_pos - pos;
        tgt_delta_pos_n.normalize();

        Vec2 result_vec = tgt_delta_pos_n;

        for (int other_suidm1 = 0; other_suidm1 < 13; other_suidm1++) {
          if (other_suidm1 == b.cfg_.suidm1) continue;
          const auto& other = roster[other_suidm1];
          const auto other_pos = Vec2{other.x, other.y};

          const auto dist_vec = other_pos - pos;
          const auto dist = dist_vec.mag();

          if (dist < b.cfg_.overlapping_collision_thr) continue;

          // Not overlapping physically, so detour if the other is at front.

          const auto watch = nearest_pmn(0.0, dist_vec.argsub(tgt_delta_pos_n));

          const auto at_front = abs(watch) < 0.25;
          if (!at_front) continue;

          const auto at_right = watch < 0.0;

          const double r = max(dist - b.cfg_.boundary_collision_thr, 1e-3);
          const double mag = 1.0 / sq(r);
          Vec2 force =
              mag * Vec2{dist_vec.arg() + (at_right ? 1.0 : -1.0) * 0.25};
          result_vec += force;
        }

        if (wf.exit_force.second < 5000) {
          result_vec += wf.exit_force.first;
        } else if (wf.exit_force.second < 10000) {
          result_vec += wf.exit_force.first.rotate(0.125);
        } else if (wf.exit_force.second < 15000) {
          result_vec += wf.exit_force.first.rotate(0.25);
        }

        if (!b.lps_.BoundMinX())
          result_vec += 1e9 * Vec2{0.0};
        else if (!b.lps_.BoundMaxX())
          result_vec += 1e9 * Vec2{0.5};
        if (!b.lps_.BoundMinY())
          result_vec += 1e9 * Vec2{0.25};
        else if (!b.lps_.BoundMaxY())
          result_vec += 1e9 * Vec2{-0.25};

        wf.cur_tgt_yaw = result_vec.arg();
        return result_vec.arg();
      };
      wd.c.tgt_yaw();  // Update aforehead.
      wd.c.stride = [] { return 0.125; };
      const auto init_didim_idx = wd.c.init_didimbal == BOOL_L ? IDX_L : IDX_R;
      wd.c.bend[init_didim_idx] = NaN;
      wd.c.speed = [] { return g::vars::speed; };
      wd.c.acclim = [] { return g::c::acclim::standard; };
      wd.c.min_stepdur = 0;
      wd.c.max_stepdur = g::c::maxdur::safe;
      wd.c.interval = 100;
      wd.c.steps = -1;
      wd.c.exit_condition = [this] {
        if (b.lps_.GetPos().dist(wf.c.tgt_pos) < 25.0) {
          ps.c.exit_to_mode = M::Idle_Init;
          return true;
        }

        if (wf.reinit) return true;

        if (b.l_.failure_.stuck || b.r_.failure_.stuck) {
#if DEBUG_FAILURE
          Pln("WTPIF: 32");
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
          wt.c.exit_to_mode = M::WalkToPosInField_Reinit;
          wf.reinit = true;
          return true;
        }

        return false;
      };
      wd.c.exit_to_mode = M::Idle_Init;
    } break;
    case M::WalkToPosInField_Reinit: {
      wf.reinit = false;
      wf.exit_force = {1e9 * Vec2{wf.cur_tgt_yaw + 0.5}, elapsedMillis{0}};
      m = M::WalkToPosInField_Init;
    } break;
    default:
      break;
  }
}
