#pragma once

#include "../helpers/timing.h"
#include "../roster/db.h"
#include "../rpl_sndrs/xbee_rs.h"
#include "../servo_units/basilisk.h"

#define XBEE_SERIAL Serial4
#define XBEE_PACKET_LEN 46  // NOT counting the 4 starting bytes.

class XbeeCommandReceiver {
  using C = Basilisk::Command;
  using M = C::Mode;

 public:
  bool Setup(Basilisk* b) {
    if (!b) {
      Serial.println("XbeeCommandReceiver: Null pointer to Basilisk");
      return false;
    }
    b_ = b;
    Serial.println("XbeeCommandReceiver: Registered reference to Basilisk");

    XBEE_SERIAL.begin(115200);
    if (!XBEE_SERIAL) {
      Serial.println("XbeeCommandReceiver: XBEE_SERIAL(Serial4) begin failed");
      return false;
    }

    Serial.println("XbeeCommandReceiver: Setup complete");
    return true;
  }

  void Run() {
    static uint8_t start = 0;
    static elapsedMicros time_since_start_us;
    static RecvBuf temp_rbuf;
    static uint8_t buf_idx;

    if (!receiving_) {
      while (XBEE_SERIAL.available() > 0 && start < 4) {
        const uint8_t rbyte = XBEE_SERIAL.read();
        if (rbyte == 255) {
          start++;
        } else {
          start = 0;
        }
      }

      if (start < 4) return;

      receiving_ = true;
      buf_idx = 0;
      got_full_packet = false;
      time_since_start_us = 0;
      globals::poll_clk_us = 0;  // Reset at start bytes anyway,
                                 // then set waiting send flag later.

#if DEBUG_PRINT_XBEE_TIMING
      Serial.println("*****XbeeCommandReceiver*****");
      Serial.print("SUID ");
      Serial.print(b_->cfg_.suid);
      Serial.print(" Start ");
      Serial.print(micros());
      Serial.println(" -> 0");
#endif
    }

    // Cannot pass this point if (waiting mode).
    // From this point, (receiving mode)

    if (time_since_start_us > timing::xb::receive_timeout_us) {
#if DEBUG_PRINT_XBEE_TIMING
      Serial.print("Wait again ");
      Serial.println(time_since_start_us);
#endif

      if (!got_full_packet) Serial.println("XBCR TIMEOUT!");

      receiving_ = false;
      start = 0;
      return;
    }

    // Cannot pass this point if (receiving mode) && (receive timeout).
    // From this point, (receiving mode) && (before receive timeout).

#if DEBUG_PRINT_XBEE_RECEIVE
    const auto prev_buf_idx = buf_idx;
#endif

    while (XBEE_SERIAL.available() > 0 && buf_idx < XBEE_PACKET_LEN) {
      temp_rbuf.raw_bytes[buf_idx++] = XBEE_SERIAL.read();
    }

#if DEBUG_PRINT_XBEE_RECEIVE
    Serial.print("Received bytes ");
    for (auto i = prev_buf_idx; i < buf_idx; i++) {
      Serial.print(temp_rbuf.raw_bytes[i]);
      Serial.print(", ");
    }
    Serial.println();
#endif

    if (buf_idx < XBEE_PACKET_LEN) return;

    // Cannot pass this point if !(got full packet).
    // From this point,
    // (receiving mode) && (before receive timeout) && (got full packet).

    got_full_packet = true;

#if DEBUG_PRINT_XBEE_TIMING
    Serial.print("Full packet received ");
    Serial.println(time_since_start_us);
#endif

    if (temp_rbuf.decoded.oneshots & (1 << ONESHOT_SaveOthersReply)) {
      // This is other's Reply. Parse and save to roster,
      // then immediately return to waiting mode.

      for (uint8_t other_suid = 1; other_suid <= 13; other_suid++) {
        if (other_suid == b_->cfg_.suid) continue;
        if (temp_rbuf.decoded.suids & (1 << (other_suid - 1))) {
          roster::db[other_suid - 1].x =
              temp_rbuf.decoded.u.save_others_reply.lpsx;
          roster::db[other_suid - 1].y =
              temp_rbuf.decoded.u.save_others_reply.lpsy;
          roster::db[other_suid - 1].yaw =
              temp_rbuf.decoded.u.save_others_reply.yaw;
          roster::updated_time[other_suid - 1] = micros();

#if DEBUG_PRINT_XBEE_RECEIVE
          Serial.print("Received Reply from SUID ");
          Serial.println(other_suid);
#endif

          break;  // There should be no Reply with multiple SUIDs.
        }
      }

#if DEBUG_PRINT_XBEE_TIMING
      Serial.print("Full packet processed ");
      Serial.println(time_since_start_us);
#endif

      receiving_ = false;
      start = 0;
      return;
    }

    else if (temp_rbuf.decoded.oneshots & (1 << ONESHOT_GlobalPoll)) {
      // This is a global Poll. SUIDs field does not matter.
      // Since poll clock is already reset to 0us at start bytes reception, just
      // set the waiting send flag now.

      XbeeReplySender::waiting_send_ = true;

#if DEBUG_PRINT_XBEE_RECEIVE
      Serial.println("Poll received, send flag set");
#endif
#if DEBUG_PRINT_XBEE_TIMING
      Serial.print("Full packet processed ");
      Serial.println(time_since_start_us);
#endif

      receiving_ = false;
      start = 0;
      return;
    }

    else {
      // This is neither a Reply nor a Poll, so it must be a normal Command.

      if (temp_rbuf.decoded.suids & (1 << (b_->cfg_.suid - 1))) {
        // This Command is for me. Copy to memory and set waiting parse flag,
        // then immediately return to waiting mode.

        memcpy(xb_cmd_.raw_bytes, temp_rbuf.raw_bytes, XBEE_PACKET_LEN);
        waiting_parse_ = true;

#if DEBUG_PRINT_XBEE_RECEIVE
        Serial.println("Command for me received, copied to memory");
#endif
#if DEBUG_PRINT_XBEE_TIMING
        Serial.print("Full packet processed ");
        Serial.println(time_since_start_us);
#endif

        receiving_ = false;
        start = 0;
        return;
      }

      else {
#if DEBUG_PRINT_XBEE_RECEIVE
        Serial.println("This Command is NOT for me");
#endif
#if DEBUG_PRINT_XBEE_TIMING
        Serial.print("Full packet processed ");
        Serial.println(time_since_start_us);
#endif

        receiving_ = false;
        start = 0;
        return;
      }
    }
  }

  inline static void Parse() {
    Serial.println("Parse begin");

    static auto& x = xb_cmd_.decoded;
    static auto& c = b_->cmd_;
    static auto& m = c.mode;

    c.oneshots = x.oneshots;
    if (c.oneshots) {
      if (c.oneshots & (1 << ONESHOT_SetBaseYaw)) {
        c.set_base_yaw.offset = static_cast<double>(x.u.set_base_yaw.offset);
      }

      return;  // Oneshot- and Mode- Commands are NOT processed from
               // a single Command since there are Oneshots that
               // require additional values.
    }

    const auto maybe_new_mode = static_cast<M>(x.mode);

    if (maybe_new_mode == M::DoPreset) {
      if (x.u.do_preset.idx[b_->cfg_.suid - 1] == 0) {
        return;  // Do not even switch Mode.  Previous DoPreset Command
                 // execution's future-chaining can be happening now.
      }

      c.do_preset.prev_mode = m;  // Save previous Mode before switching.
    }

    m = maybe_new_mode;
    switch (m) {
      case M::DoPreset: {
        c.do_preset.idx = x.u.do_preset.idx[b_->cfg_.suid - 1];

        // Serial.print("Copied to memory, Preset index ");
        // Serial.print(c.do_preset.idx);
        // Serial.println();
      } break;
      case M::Pivot_Init: {
        c.pivot.bend[IDX_L] = static_cast<double>(x.u.pivot.bend_l);
        c.pivot.bend[IDX_R] = static_cast<double>(x.u.pivot.bend_r);
        c.pivot.didimbal = static_cast<bool>(x.u.pivot.didimbal);
        c.pivot.speed = static_cast<double>(x.u.pivot.speed);
        c.pivot.stride = static_cast<double>(x.u.pivot.stride);
        c.pivot.tgt_yaw = [](Basilisk*) {
          return static_cast<double>(x.u.pivot.tgt_yaw);
        };
        c.pivot.min_dur = 0;
        c.pivot.max_dur = 6000;
        c.pivot.exit_condition = nullptr;
        c.pivot.acclim = 1.0;
        c.pivot.exit_to_mode = M::Idle_Init;
      } break;
      default: {
        // Serial.print("XbeeCommandReceiver: Mode ");
        // Serial.print(x.mode);
        // Serial.print(" is NOT registered");
        // Serial.println();
      } break;
    }
  }

  inline static union RecvBuf {
    struct Decoded {
      uint16_t suids;
      uint8_t oneshots;
      uint8_t mode;
      union {
        struct {
          float offset;
        } __attribute__((packed)) set_base_yaw;
        struct {
          float lpsx;
          float lpsy;
          float yaw;
        } __attribute__((packed)) save_others_reply;
        struct {
          uint16_t idx[13];  // The Goguma version of DoPreset protocol.
        } __attribute__((packed)) do_preset;
        struct {
          float bend_l;
          float bend_r;
          uint8_t didimbal;
          float speed;
          float stride;
          float tgt_yaw;
        } __attribute__((packed)) pivot;
      } u;
    } __attribute__((packed)) decoded;
    uint8_t raw_bytes[XBEE_PACKET_LEN];
  } xb_cmd_;

  inline static bool receiving_ = false;
  inline static bool waiting_parse_ = false;
  inline static bool waiting_xb_rpl_send_ = false;

  // Flags for LedReplySender
  inline static bool led_got_start_bytes = false;
  inline static bool got_full_packet = false;
  inline static bool led_got_my_cmd = false;
  inline static bool led_timeout_miss = false;

 private:
  inline static Basilisk* b_ = nullptr;
};
