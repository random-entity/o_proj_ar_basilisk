#pragma once

#include "../globals/moteus_fmt.h"
#include "../helpers/clamped.h"
#include "canfd_drivers.h"

class Servo : public Moteus {
 public:
  Servo(const int& id, uint8_t bus,  //
        const PmFmt* const pm_fmt, const QFmt* const q_fmt)
      : Moteus{canfd_drivers[bus - 1],
               [&]() {
                 Options options;
                 options.id = id;
                 options.default_query = false;  // Query right before,
                                                 // not along with Command.
                                                 // Instead make sure to
                                                 // pass format_override
                                                 // argument whenever Querying.
                 return options;
               }()},
        id_{id},
        pm_fmt_{pm_fmt},
        q_fmt_{q_fmt} {}

  bool SetQuery() {
    const auto prev_rpl = last_result().values;

    const auto got_rpl = static_cast<Moteus*>(this)->SetQuery(q_fmt_);

    if (!got_rpl) return false;

    const auto rpl = GetReply();

    failure_.encoder_invalid =
        (static_cast<uint8_t>(rpl.extra[1].value) != 0xF);
    failure_.aux2pos_range_invalid =
        (rpl.abs_position != Phi{rpl.abs_position});
    failure_.aux2pos_frozen = (prev_rpl.abs_position == rpl.abs_position);
    failure_.torque_too_high =
        (abs(rpl.torque) >
         0.9 * moteus_fmt::pm_cmd_template.maximum_torque);
    return failure_.Exists();
  }

  // Aux2 position uncoiled.
  QRpl GetReply() {
    auto rpl = last_result().values;
    // Convert aux2 encoder reading in [0, 1] to phi in [-0.5, 0.5]
    if (rpl.abs_position > 0.5) rpl.abs_position -= 1.0;
    return rpl;
  }

  void SetPosition(const PmCmd& cmd) {
    static_cast<Moteus*>(this)->SetPosition(cmd, pm_fmt_);
  }

  struct {
    bool encoder_invalid = false;
    bool aux2pos_range_invalid = false;
    bool aux2pos_frozen = false;
    bool torque_too_high = false;

    bool Exists() const {
      return encoder_invalid ||        //
             aux2pos_range_invalid ||  //
             aux2pos_frozen ||         //
             torque_too_high;
    }
  } failure_;

 private:
  const int id_;
  const PmFmt* const pm_fmt_;
  const QFmt* const q_fmt_;

 public:
  void Print() {
    const auto rpl = GetReply();
    Serial.print(id_);
    Serial.print(F(" -> t "));
    Serial.print(millis());
    Serial.print(F(" / mod "));
    Serial.print(static_cast<int>(rpl.mode));
    Serial.print(F(" / pos "));
    Serial.print(rpl.position, 3);
    Serial.print(F(" / vel "));
    Serial.print(rpl.velocity, 3);
    Serial.print(F(" / trq "));
    Serial.print(rpl.torque, 3);
    Serial.print(F(" / qcr "));
    Serial.print(rpl.q_current);
    Serial.print(F(" / dcr "));
    Serial.print(rpl.d_current);
    Serial.print(F(" / a2p "));
    Serial.print(rpl.abs_position, 3);
    Serial.print(F(" / mtp "));
    Serial.print(rpl.motor_temperature);
    Serial.print(F(" / tjc "));
    Serial.print(rpl.trajectory_complete);
    Serial.print(F(" / hom "));
    Serial.print(static_cast<int>(rpl.home_state));
    Serial.print(F(" / vlt "));
    Serial.print(rpl.voltage);
    Serial.print(F(" / tmp "));
    Serial.print(rpl.temperature);
    Serial.print(F(" / flt "));
    Serial.print(rpl.fault);
    Serial.print(F(" / a2v "));
    Serial.print(rpl.extra[0].value, 3);
    Serial.print(F(" / evl "));
    Serial.print(static_cast<uint8_t>(rpl.extra[1].value), BIN);
    Serial.println();
  }
};
