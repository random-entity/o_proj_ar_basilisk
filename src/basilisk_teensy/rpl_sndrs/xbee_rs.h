#pragma once

#include "../helpers/timing.h"
#include "../servo_units/basilisk.h"

#ifndef XBEE_SERIAL
#define XBEE_SERIAL Serial4
#endif
#define XBEE_PACKET_LEN_INCLUDING_START_BYTES (50)

class XbeeReplySender {
 public:
  inline static Basilisk* b_;
  inline static bool waiting_send_ = false;

  // Should be called before use.
  inline static bool Setup(Basilisk* b) {
    if (!b) {
      Pln("XbeeReplySender: Null reference to Basilisk");
      return false;
    }
    b_ = b;
    xb_rpl_.decoded.suids = 1 << (b->cfg_.suid - 1);
    Pln("XbeeReplySender: Setup complete");
    return true;
  }

  // Should be run continuously
  inline static void Run() {
    using namespace timing::xb;
    static const auto sndtim_us = suid_to_send_time_us.at(b_->cfg_.suid);

    if (!waiting_send_) return;
    if (globals::poll_clk_us < sndtim_us) return;
    waiting_send_ = false;
    if (globals::poll_clk_us >= sndtim_us + send_timeout_us) {
#if DEBUG_PRINT_XBEE_SEND
      Pln("XbRS timeout");
#endif
      return;
    }

#if DEBUG_PRINT_XBEE_TIMING
    Pln("********");
    Pln("My Reply");
    P("Begin ");
    Serial.println(globals::poll_clk_us);
#endif

    Send();

#if DEBUG_PRINT_XBEE_TIMING
    P("Done ");
    Serial.println(globals::poll_clk_us);
#endif
  }

  inline static void Send() {
    xb_rpl_.decoded.mode = static_cast<uint8_t>(*b_->rpl_.mode);
    xb_rpl_.decoded.lpsx = static_cast<float>(*b_->rpl_.lpsx);
    xb_rpl_.decoded.lpsy = static_cast<float>(*b_->rpl_.lpsy);
    xb_rpl_.decoded.yaw = static_cast<float>(b_->rpl_.yaw());
    xb_rpl_.decoded.phi_l = static_cast<float>(b_->rpl_.phi_l());
    xb_rpl_.decoded.phi_r = static_cast<float>(b_->rpl_.phi_r());

    XBEE_SERIAL.write(xb_rpl_.raw_bytes, XBEE_PACKET_LEN_INCLUDING_START_BYTES);
  }

  inline static union SendBuf {
    struct Decoded {
      const uint32_t start_bytes;  // Start bytes are included!
      uint16_t suids;
      const uint8_t oneshots;
      uint8_t mode;
      float lpsx;
      float lpsy;
      float yaw;
      float phi_l;
      float phi_r;
    } __attribute__((packed)) decoded;
    uint8_t raw_bytes[XBEE_PACKET_LEN_INCLUDING_START_BYTES];

    SendBuf()
        : decoded{.start_bytes{static_cast<uint32_t>(-1)},
                  .oneshots{1 << ONESHOT_SaveOthersReply}} {}
  } xb_rpl_;
};
