#pragma once

#include <Arduino.h>
#include <elapsedMillis.h>

#include "../globals/moteus_fmt.h"
#include "../globals/serials.h"
#include "../helpers/clamped.h"
#include "../helpers/rpl_extra.h"
#include "../helpers/using_moteus.h"
#include "canfd_drivers.h"

class Servo : public Moteus {
 public:
  Servo(const int& id, const int& bus,  //
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
        q_fmt_{q_fmt} {
    InitializeCanFdDriver(bus);
  }

  bool SetQuery() {
    const auto prev_rpl = last_result().values;

    failure_.query_failed = !(static_cast<Moteus*>(this)->SetQuery(q_fmt_));

    if (failure_.query_failed) return false;

    const auto rpl = GetReply();

    failure_.encoder_invalid =
        (static_cast<uint8_t>(GetExtraValue(rpl, kEncoderValidity)) != 0xF);
    failure_.aux2pos_invalid_range =
        (rpl.abs_position != Phi{rpl.abs_position});
    failure_.aux2pos_frozen =
        (abs(prev_rpl.abs_position - rpl.abs_position) < 1e-4);
    failure_.stuck = (abs(GetExtraValue(rpl, kControlVelocity)) > 1e-2) &&
                     (abs(GetExtraValue(rpl, kControlVelocityError)) > 1e-1);
    failure_.overtorque =
        (abs(rpl.torque) > 0.9 * GetExtraValue(rpl, kCommandPositionMaxTorque));

    return failure_.Exists();
  }

  // Aux2 position uncoiled.
  QRpl GetReply() const {
    auto rpl = last_result().values;
    // Convert aux2 encoder reading in [0, 1] to phi in [-0.5, 0.5]
    if (rpl.abs_position > 0.5) rpl.abs_position -= 1.0;
    return rpl;
  }

  void SetPosition(const PmCmd& cmd) {
    static_cast<Moteus*>(this)->SetPosition(cmd, pm_fmt_);
  }

  struct Failure {
    inline static constexpr int num_items = 6;

    struct Item {
      bool present = false;
      elapsedMillis since = 0;
      const uint32_t persist_thr;

      Item(const uint32_t& _persist_thr) : persist_thr{_persist_thr} {}

      Item& operator=(const bool& val) {
        present = val;
        if (!present) since = 0;
        return *this;
      }

      operator bool() const { return present && since >= persist_thr; }
    } items[num_items] = {{0}, {0}, {0}, {100}, {250}, {500}};

    Item& query_failed = items[0];
    Item& encoder_invalid = items[1];
    Item& aux2pos_invalid_range = items[2];
    Item& aux2pos_frozen = items[3];
    Item& stuck = items[4];
    Item& overtorque = items[5];

    bool Exists() const {
      for (int i = 0; i < num_items; i++) {
        if (items[i]) return true;
      }
      return false;
    }

    uint8_t Export() const {
      static uint8_t result = 0;
      for (int i = 0; i < num_items; i++) {
        if (items[i]) result |= (1 << i);
      }

      return result;
    }
  } failure_;

  //  private:
  const int id_;
  const PmFmt* const pm_fmt_;
  const QFmt* const q_fmt_;

 public:
  void Print() const {
#if ENABLE_SERIAL
    const auto rpl = GetReply();
    P("Servo ");
    Serial.print(id_);
    P(" -> t ");
    Serial.print(millis());
    P(" | mod ");
    Serial.print(static_cast<int>(rpl.mode));
    P(" | pos ");
    Serial.print(rpl.position, 4);
    P(" | vel ");
    Serial.print(rpl.velocity, 4);
    P(" | trq ");
    Serial.print(rpl.torque, 4);
    P(" | qcr ");
    Serial.print(rpl.q_current);
    P(" | dcr ");
    Serial.print(rpl.d_current);
    P(" | a2p ");
    Serial.print(rpl.abs_position, 4);
    P(" | mtp ");
    Serial.print(rpl.motor_temperature);
    P(" | tjc ");
    Serial.print(rpl.trajectory_complete);
    P(" | hom ");
    Serial.print(static_cast<int>(rpl.home_state));
    P(" | vlt ");
    Serial.print(rpl.voltage);
    P(" | tmp ");
    Serial.print(rpl.temperature);
    P(" | flt ");
    Serial.print(rpl.fault);
    P(" | ver ");
    Serial.print(GetExtraValue(rpl, kControlVelocityError), 4);
    P(" | a2v ");
    Serial.print(GetExtraValue(rpl, kEncoder1Velocity), 4);
    P(" | evl ");
    Serial.print(static_cast<uint8_t>(GetExtraValue(rpl, kEncoderValidity)),
                 BIN);
    Serial.println();
#endif
  }
};
