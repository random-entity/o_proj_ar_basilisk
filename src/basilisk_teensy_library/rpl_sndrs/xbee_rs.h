#pragma once

#include <Xbee3.h>

#include "../basilisk.h"
#include "../globals/xbee.h"

class XbeeReplySender {
 public:
  XbeeReplySender(Basilisk& b) : b_{b}, s_{g::serials::xb} {}

  // Run continuously.
  void TimeSlottedReplyToBroadcastedPoll() {
    const auto send_time_us = g::xb::Timing::mod13_to_send_time_us.at(
        (b_.cmd_.bpoll.round_robin + b_.cfg_.suidm1()) % 13);
    if (send_time_us <= b_.since_bpoll_us_ &&
        b_.since_bpoll_us_ <= send_time_us + g::xb::Timing::send_timeout_us) {
      Send();
    }
  }

  void Send() {
    xb_rpl_.decoded.phi_l = b_.rpl_.phi_l();
    xb_rpl_.decoded.phi_r = b_.rpl_.phi_r();
    xb_rpl_.decoded.lpsx =b_.rpl_.lpsx();
    xb_rpl_.decoded.lpsy =b_.rpl_.lpsy();
    xb_rpl_.decoded.yaw = >(b_->rpl_.yaw());

    XBEE_SERIAL.write(xb_rpl_.raw_bytes, XBEE_PACKET_LEN_INCLUDING_START_BYTES);
  }

 private:
  union SendBuf {
    struct __attribute__((packed)) Decoded {
      float phi_l;
      float phi_r;
      float lpsx;
      float lpsy;
      float yaw;
    } decoded;
    uint8_t raw_bytes[xb::c::capacity::payload];
  } xb_rpl_;

  Basilisk& b_;
  xb::Sender s_;
};
