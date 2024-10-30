#pragma once

#include "../basilisk.h"

namespace tests {
using M = Basilisk::Command::Mode;

void Temp1(Basilisk& b_) {
  b_.CommandBoth([](Servo& s) {
    s.SetPosition([] {
      auto cmd = g::moteus_fmt::pm_cmd_template;
      cmd.position = NaN;
      cmd.velocity = 0.2;
      // cmd.maximum_torque = 0.02;
      cmd.watchdog_timeout = NaN;
      return cmd;
    }());
  });
}

void SetPhis(Basilisk& b) {
  auto& m = b.cmd_.mode;
  auto& ph = b.cmd_.set_phis;

  m = M::SetPhis_Init;
  ph.tgt_phi[IDX_L] = [] { return 0.0; };
  ph.tgt_phi[IDX_R] = [] { return 0.0; };
  ph.tgt_phispeed[IDX_L] = [] { return 0.1; };
  ph.tgt_phispeed[IDX_R] = [] { return 0.1; };
  ph.tgt_phiacclim[IDX_L] = [] { return g::c::acclim::standard; };
  ph.tgt_phiacclim[IDX_R] = [] { return g::c::acclim::standard; };
  ph.damp_thr = g::c::phithr::damp::standard;
  ph.fix_thr = g::c::phithr::fix::standard;
  ph.fixing_cycles_thr = 1;
  ph.min_dur = 0;
  ph.max_dur = -1;
  ph.exit_condition = [] { return false; };
  ph.exit_to_mode = M::Idle_Init;
}

void Pivot(Basilisk& b) {
  auto& m = b.cmd_.mode;
  auto& pv = b.cmd_.pivot;

  m = M::Pivot_Init;
  pv.didimbal = BOOL_L;
  pv.tgt_yaw = [] { return NaN; };
  pv.stride = [] { return 0.125; };
  pv.bend[IDX_L] = 0.0;
  pv.bend[IDX_R] = -0.125;
  pv.speed = [] { return g::c::speed::fast; };
  pv.acclim = [] { return g::c::acclim::standard; };
  pv.min_dur = 2000;
  pv.max_dur = -1;
  pv.exit_to_mode = M::Idle_Init;
}

void PivSpin(Basilisk& b) {
  auto& m = b.cmd_.mode;
  auto& sp = b.cmd_.piv_spin;

  m = M::PivSpin;
  sp.didimbal = BOOL_L;
  sp.dest_yaw = NaN;
  sp.exit_thr = NaN;
  sp.stride = 0.125;
  sp.bend[IDX_L] = 0.0;
  sp.bend[IDX_R] = 0.0;
  sp.speed = [] { return g::c::speed::normal; };
  sp.acclim = [] { return g::c::acclim::standard; };
  sp.min_stepdur = 0;
  sp.max_stepdur = -1;
  sp.interval = 0;
  sp.steps = -1;
}

void Diamond(Basilisk& b) {
  auto& m = b.cmd_.mode;
  auto& c = b.cmd_.diamond;

  m = M::Diamond;
  c.init_didimbal = BOOL_L;
  c.init_stride = 0.3;
  c.speed = g::c::speed::fast;
  c.acclim = g::c::acclim::standard;
  c.min_stepdur = 0;
  c.max_stepdur = -1;
  c.interval = 100;
  c.steps = -1;
}

void WalkToDir(Basilisk& b) {
  auto& m = b.cmd_.mode;
  auto& wd = b.cmd_.walk_to_dir;

  m = M::WalkToDir;
  wd.init_didimbal = BOOL_L;
  wd.tgt_yaw = [] { return 0.0; };
  wd.stride = [] { return 0.125; };
  wd.bend[IDX_L] = 0.0;
  wd.bend[IDX_R] = 0.0;
  wd.speed = [] { return g::c::speed::normal; };
  wd.acclim = [] { return g::c::acclim::standard; };
  wd.min_stepdur = 1000;
  wd.max_stepdur = 3000;
  wd.interval = 0;
  wd.steps = -1;
}

void WalkToPos(Basilisk* b) {
  auto& m = b->cmd_.mode;
  auto& c = b->cmd_.walk_to_pos;

  m = M::WalkToPos;
  c.init_didimbal = BOOL_L;
  c.tgt_pos = Vec2{(b->cfg_.lps.minx + b->cfg_.lps.maxx) * 0.5,
                   (b->cfg_.lps.miny + b->cfg_.lps.maxy) * 0.5};
  c.dist_thr = 30;
  c.stride = 0.125;
  c.bend[IDX_L] = 0.0;
  c.bend[IDX_R] = 0.0;
  c.speed = g::c::speed::normal;
  c.acclim = g::c::acclim::standard;
  c.min_stepdur = 1000;
  c.max_stepdur = 3000;
  c.interval = 0;
  c.steps = -1;
}

// void Sufi(Basilisk* b) {
//   auto& m = b->cmd_.mode;
//   auto& c = b->cmd_.sufi;

//   m = M::Sufi;
//   c.init_didimbal = BOOL_L;
//   c.dest_yaw = NaN;
//   c.exit_thr = NaN;
//   c.stride = 0.125;
//   c.bend[IDX_L] = 0.0;
//   c.bend[IDX_R] = 0.0;
//   c.speed = g::c::speed::normal;
//   c.acclim = g::c::acclim::standard;
//   c.min_stepdur = 1000;
//   c.max_stepdur = 3000;
//   c.interval = 0;
//   c.steps = -1;
// }

void BounceWalk(Basilisk& b) {
  auto& m = b.cmd_.mode;
  auto& c = b.cmd_.bounce_walk;

  m = M::BounceWalk_Init;
  c.init_tgt_yaw = 0.0;  //  random(360) / 360.0;
}

void WalkToPosInField(Basilisk* b) {
  auto& m = b->cmd_.mode;
  auto& c = b->cmd_.walk_to_pos_in_field;

  m = M::WalkToPosInField_Init;
  c.tgt_pos = Vec2{500.0, 500.0};
}

}  // namespace tests
