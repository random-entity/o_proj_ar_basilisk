#pragma once

#include <Arduino.h>
#include <Smoothed.h>
#include <elapsedMillis.h>

#include "../globals/serials.h"
#include "../helpers/vec2.h"

/* Length unit of incoming bytes from the LPS board is '10cm', so the field
 * `uin8_t dists_raw_[3]` which saves the raw values follows this unit, but the
 * rest of the program assumes 'cm' as length unit, for convenience in intuitive
 * choreography.
 *
 * Assumptions:
 * - Anchors ABC and the LPS board on Basilisk are on the same height.
 * - Anchor A = (0, 0); Anchor B = (c, 0); Anchor C = (x_c, y_c);
 *   with c > 0; y_c > 0.
 * - Basilisk never reaches beyond Anchor C in y axis, i.e. y < y_c.
 *
 * Trilateration:
 * (1):         x^2 + y^2         = a^2
 * (2):   (x - c)^2 + y^2         = b^2
 * (3): (x - x_c)^2 + (y - y_c)^2 = c^2
 *
 * (4) = (1) - (2): 2cx - c^2 = a^2 - b^2
 *                  x = (a^2 - b^2 + c^2) / (2c)
 * From (3): y = y_c +- sqrt(c^2 - (x - x_c)^2)
 *           is computable since we already have computed x at (4),
 *           and we take minus since we assumed y < y_c. */
class Lps {
 public:
  Lps(const double& c, const double& x_c, const double& y_c,  //
      const double& minx, const double& maxx,                 //
      const double& miny, const double& maxy)
      : cfg_{.c = c,
             .x_c = x_c,
             .y_c = y_c,
             .minx = minx,
             .maxx = maxx,
             .miny = miny,
             .maxy = maxy} {
    for (auto& dist_sm : dists_sm_) dist_sm.begin(SMOOTHED_AVERAGE, 5);

#if DEBUG_SETUP
    Pln("LPS: Setup complete");
#endif
  }

  // Call continuously to immediately receive incoming sensor data
  // and prevent Serial buffer overflow.
  void Run() {
#if MOCK_LPS
    x_ = micros() * 1e-7;
    y_ = micros() * 1e-7;
    return;
#endif

    if (ser_.available() >= 60) {
      for (int i = 0; i < 60; i++) ser_.read();
    }

    if (!rx_start_) {
      if (!(ser_.available() && ser_.read() == 255)) return;
      if (!(ser_.available() && ser_.read() == 2)) return;
      rx_start_ = true;
    }

    if (ser_.available() < 4) return;

    error_.matome = 0;
    for (int i = 0; i < 3; i++) {
      const auto raw = ser_.read();
      if (raw < 250) {
        dists_raw_[i] = raw;
        dists_sm_[i].add(10.0 * raw);
      } else {
        error_.bytes[i] = raw;
      }
    }
    latency_ = ser_.read();
    if (!error_.matome) SetXY();
    since_raw_update_ = 0;

    rx_start_ = false;
  }

 private:
  void SetXY() {
    const auto a = dists_sm_[0].get();
    const auto b = dists_sm_[1].get();
    const auto c = dists_sm_[2].get();

    x_ = (sq(a) - sq(b) + sq(cfg_.c)) / (2 * cfg_.c);

    const auto temp = sq(c) - sq(x_ - cfg_.x_c);
    if (temp >= 0) {
      y_ = cfg_.y_c - sqrt(temp);
    } else {
      y_ = cfg_.y_c;
    }

    since_xy_update_ = 0;
  }

 public:
  Vec2 GetPos() { return Vec2{x_, y_}; }

  bool BoundMinX() { return cfg_.minx < x_; }
  bool BoundMaxX() { return x_ < cfg_.maxx; }
  bool BoundMinY() { return cfg_.miny < y_; }
  bool BoundMaxY() { return y_ < cfg_.maxy; }
  bool Bound() {
    return BoundMinX() && BoundMaxX() && BoundMinY() && BoundMaxY();
  }

  HardwareSerial& ser_ = g::serials::lps;
  bool rx_start_ = false;
  uint8_t dists_raw_[3] = {0, 0, 0};
  union {
    uint8_t bytes[3];
    uint32_t matome = 0;
  } error_;
  uint8_t latency_ = 0;
  elapsedMillis since_raw_update_ = 0;
  Smoothed<double> dists_sm_[3];
  double x_ = 0.0, y_ = 0.0;
  elapsedMillis since_xy_update_ = 0;
  const struct {
    double c, x_c, y_c;
    double minx, maxx, miny, maxy;
  } cfg_;
};
