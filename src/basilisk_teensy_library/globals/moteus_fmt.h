#pragma once

#include <Arduino.h>

#include "../helpers/using_moteus.h"

namespace g::moteus_fmt {

const PmFmt pm_fmt{
    .position = kFloat,
    .velocity = kFloat,
    .feedforward_torque = kIgnore,
    .kp_scale = kIgnore,
    .kd_scale = kIgnore,
    .maximum_torque = kFloat,  // Do NOT modify for overtorque detection.
    .stop_position = kIgnore,  // Do NOT use!
    .watchdog_timeout = kFloat,
    .velocity_limit = kFloat,
    .accel_limit = kFloat,
    .fixed_voltage_override = kIgnore,
};

const PmCmd pm_cmd_template{
    .position = NaN,
    .velocity = 0.0,
    .feedforward_torque = 0.0,
    .kp_scale = 1.0,
    .kd_scale = 1.0,
    .maximum_torque = 0.5,
    .stop_position = NaN,  // Do NOT use!
    .watchdog_timeout = 0.1,
    .velocity_limit = 21.0,
    .accel_limit = 210.0,
    .fixed_voltage_override = NaN,
};

const QFmt q_fmt{[] {
  QFmt fmt;
  fmt.mode = kInt8;
  fmt.position = kFloat;
  fmt.velocity = kFloat;
  fmt.torque = kFloat;
  fmt.q_current = kFloat;
  fmt.d_current = kFloat;
  fmt.abs_position = kFloat;
  fmt.motor_temperature = kFloat;
  fmt.trajectory_complete = kInt8;
  fmt.home_state = kIgnore;
  fmt.voltage = kFloat;
  fmt.temperature = kFloat;
  fmt.fault = kInt8;

  /* Querying 3 or more Extras result in Query failure for all Items
   * for unknown reason. */
  /* kControlPositionError: The key for non-LPS-based collision detection.
       The control position is the desired position, calculated even when
       velocity-Commanding. */
  /* But will this work with the 'max position slip' set to non-NaN value?
   * Maybe we can utilize that value to our advantage. */
  fmt.extra[0] = {
      .register_number = kControlPositionError,
      .resolution = kFloat,
  };
  /* kEncoderValidity: A vital must-have. */
  fmt.extra[1] = {
      .register_number = kEncoderValidity,
      .resolution = kInt8,
  };

  /* Considerations for Querying the right value for non-LPS-based collision
   * detection while number of Extra Query Items restricted to 2:
   * - kCommandPositionMaxTorque: Just use pm_cmd_template.maximum_torque.
   * - kControlVelocity: Just use pm_cmd.velocity whenever Pm-Commanding.
   * - kControlVelocityError: Just use pm_cmd.velocity - GetReply().velocity
   *     whenever Pm-Commanding.  The QRpl velocity value is too noisy to use
   *     for collision detection anyway.
   * - kEncoder1Velocity: Not sure if this value is not noisy enough. */

  return fmt;
}()};

}  // namespace g::moteus_fmt
