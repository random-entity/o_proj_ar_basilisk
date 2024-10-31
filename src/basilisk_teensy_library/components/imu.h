#pragma once

#include <Arduino.h>
#include <elapsedMillis.h>

#include "../globals/serials.h"
#include "../helpers/vec2.h"

/* Angle unit of incoming data from the EBIMU board are in 'degrees', and
 * between -180.0 and 180.0, but the rest of the program assumes 'revolutions'
 * as angle unit for compatibility with mjbots moteus API.
 * The field `double euler_[2]` stores angles in revolutions,
 * between -0.5 and 0.5, and the field `yaw_revs_` tracks full revolutions
 * in yaw so we can compute its 'uncoiled' value.
 * All yaw values including returns are uncoiled except `euler_[2]`. */
class Imu {
 public:
  Imu() {
#if DEBUG_SETUP
    Pln("IMU: Setup complete");
#endif
  }

  // Call continuously to immediately receive to incoming sensor data
  // and prevent Serial buffer overflow and correctly track revolutions.
  void Run() {
#if MOCK_IMU
    euler_[2] = micros() * 1e-7;
    return;
#endif

    static const auto increment_idx = [&] {
      if (++buf_idx_ >= buf_size_) buf_idx_ = 0;
    };

    const auto rbytes = ser_.available();
    for (int i = 0; i < rbytes; i++, increment_idx()) {
      buf_[buf_idx_] = ser_.read();
      if (buf_[buf_idx_] == '\n') {
        char* temp[3];
        temp[0] = strtok(buf_, ",");
        temp[1] = strtok(nullptr, ",");
        temp[2] = strtok(nullptr, ",");
        if ([&] {
              for (const auto* s : temp) {
                if (!s) return false;
              }
              return true;
            }()) {
          const auto prev_yaw_coiled = euler_[2];
          for (int i = 0; i < 3; i++) {
            euler_[i] = atof(temp[i]) / 360.0;
          }
          euler_[2] *= -1.0;  // Right hand thumbs up.
          const auto delta_yaw_coiled = euler_[2] - prev_yaw_coiled;
          if (delta_yaw_coiled > 0.5) {
            yaw_revs_--;
          } else if (delta_yaw_coiled < -0.5) {
            yaw_revs_++;
          }
          since_update_ = 0;
        }
      } else if (buf_[buf_idx_] == '*') {
        buf_idx_ = -1;
      }
    }
  }

  // rel == false: Absolute, true: Relative to Base
  double GetYaw(const bool rel = true) {
    if (rel) {
      return euler_[2] + yaw_revs_ - base_yaw_;
    } else {
      return euler_[2] + yaw_revs_;
    }
  }

  void SetBaseYaw(const double& offset = 0.0) {
    base_yaw_ = GetYaw(false) - offset;
  }

  void SaveRealBaseYaw() { real_base_yaw_saved_ = base_yaw_; }
  void RestoreRealBaseYaw() { base_yaw_ = real_base_yaw_saved_; }

  HardwareSerial& ser_ = g::serials::imu;
  inline static constexpr int buf_size_ = 64;
  char buf_[buf_size_];
  int buf_idx_ = 0;
  double euler_[3] = {0.0};  // [0]: roll, [1]: pitch, [2]: yaw
  int yaw_revs_ = 0;
  double base_yaw_ = 0.0;
  double real_base_yaw_saved_ = 0.0;
  elapsedMillis since_update_;
};
