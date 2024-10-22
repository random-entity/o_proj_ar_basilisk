#pragma once

#include <Arduino.h>
#include <elapsedMillis.h>

#include "../globals/serials.h"
#include "../helpers/serial_print.h"

/* Angle unit of incoming data from the EBIMU board are in 'degrees', and
 * between -180.0 and 180.0, but the rest of the program assumes 'revolutions'
 * as angle unit for compatibility with mjbots moteus API.
 * The field `double euler_[2]` stores angles in revolutions,
 * between -0.5 and 0.5, and the field `yaw_revs_` tracks full revolutions
 * in yaw so we can compute its 'uncoiled' value.
 * All yaw values including returns are uncoiled except `euler_[2]`. */
class Imu {
 public:
  // Must be called before use.
  bool Setup() {
    IMU_SERIAL.begin(IMU_SERIAL_BAUDRATE);
    delay(SERIAL_BEGIN_WAIT_TIME_MS);
    if (!IMU_SERIAL) {
#if DEBUG_INITIALIZATION
      Pln("IMU: IMU_SERIAL(Serial2) begin failed");
#endif
      return false;
    }

#if DEBUG_INITIALIZATION
    Pln("IMU: Setup complete");
#endif
    return true;
  }

  // Should be called continuously to immediately receive to
  // incoming sensor data and prevent Serial buffer overflow
  // and correctly track full revolutions.
  void Run() {
    static const int buf_size = 64;
    static char buf[buf_size];
    static int buf_idx = 0;
    static const auto increment_idx = [&] {
      if (++buf_idx >= 64) buf_idx = 0;
    };

    const auto rbytes = IMU_SERIAL.available();
    for (int i = 0; i < rbytes; i++, increment_idx()) {
      buf[buf_idx] = IMU_SERIAL.read();
      if (buf[buf_idx] == '\n') {
        char* temp[3];
        temp[0] = strtok(buf, ",");
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
          euler_[2] *= -1.0;  // Translate to ((vertical up) = +z)
                              // right hand system.
          const auto delta_yaw_coiled = euler_[2] - prev_yaw_coiled;
          if (delta_yaw_coiled > 0.5) {
            yaw_revs_--;
          } else if (delta_yaw_coiled < -0.5) {
            yaw_revs_++;
          }
          since_update_ = 0;
        }
      } else if (buf[buf_idx] == '*') {
        buf_idx = -1;
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

  double euler_[3];  // [0]: roll, [1]: pitch, [2]: yaw
  int yaw_revs_ = 0;
  double base_yaw_ = 0.0;
  elapsedMillis since_update_;
};
