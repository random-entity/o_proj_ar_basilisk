#pragma once

#include "_meta.h"

void PPPShooter::LookRelativeToCenter(int senw) {
  m = M::Sufi;
  auto& c = b.cmd_.sufi;

  static const auto center = Vec2{(b.cfg_.lps.minx + b.cfg_.lps.maxx) * 0.5,
                                  (b.cfg_.lps.miny + b.cfg_.lps.maxy) * 0.5};

  c.init_didimbal = BOOL_L;
  c.dest_yaw = nearest_pmn(b.imu_.GetYaw(true),
                           (center - b.lps_.GetPos()).arg() + ((senw - 1) / 8.0));
  c.exit_thr = 0.01;
  c.stride = 30.0 / 360.0;
  bool dest_is_greater = c.dest_yaw > b.imu_.GetYaw(true);
  if (!dest_is_greater) {
    c.stride *= -1.0;
  }
  c.bend[IDX_L] = 0.0;
  c.bend[IDX_L] = 0.0;
  c.speed = [] { return g::vars::speed; };
  c.acclim = [] { return g::c::acclim::standard; };
  c.min_stepdur = 0;
  c.max_stepdur = g::c::maxdur::safe;
  c.interval = 300;
  c.steps = -1;
}

void PPPShooter::LookSolo() {
  m = M::Sufi;
  auto& c = b.cmd_.sufi;

  static const auto solo = Vec2{440, -110};

  c.init_didimbal = BOOL_L;
  c.dest_yaw = nearest_pmn(b.imu_.GetYaw(true), (solo - b.lps_.GetPos()).arg());
  c.exit_thr = 0.01;
  c.stride = 30.0 / 360.0;
  bool dest_is_greater = c.dest_yaw > b.imu_.GetYaw(true);
  if (!dest_is_greater) {
    c.stride *= -1.0;
  }
  c.bend[IDX_L] = 0.0;
  c.bend[IDX_L] = 0.0;
  c.speed = [] { return g::vars::speed; };
  c.acclim = [] { return g::c::acclim::standard; };
  c.min_stepdur = 0;
  c.max_stepdur = g::c::maxdur::safe;
  c.interval = 100;
  c.steps = -1;
}
