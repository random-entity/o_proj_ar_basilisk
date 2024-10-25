#pragma once

#include <Xbee3.h>

#include "../basilisk.h"
#include "../globals/serials.h"
#include "../globals/xbee.h"
#include "../roster.h"

/* This class is capable of handling the following type of messages:
 * - Broadcasted parameterized-preset-protocol (B-PPP) Commands:
 *   <- Is iff (source is Commander) && payload[O/M] == M::BPPP.
 *   -> Set waiting injection flag to inject at next ExecutionCycle.
 *     * Waiting is for time synchronization with ModeRunners.
 * - Time-slotted Poll Commands:
 *   <- Is iff (source is Commander) && payload[O/M] == O::TimeSlottedPoll.
 *   -> Reset poll clock for next ReplySend.
 * - Fellow Replies:
 *   <- Is iff (source is Fellow).
 *   -> Save to Roster immediately. */
class XbeeCommandReceiver {
  using C = Basilisk::Command;
  using M = C::Mode;
  using O = C::Oneshot;

 public:
  XbeeCommandReceiver(Basilisk& b)
      : b_{b},
        r_{g::serials::xb(),
           [this](xb::ReceivePacket& packet, int payload_size) {
             Parse(packet, payload_size);
           }} {}

  // Call continuously.
  void Run() { r_.Run(); }

  void Parse(xb::ReceivePacket& packet, int payload_size) {
    const auto srcnid_it = g::xb::addr::to_nid.find(packet.src_addr());
    if (srcnid_it == g::xb::addr::to_nid.end()) return;
    const auto nodeid = srcnid_it->second;

    //////////////////
    // Fellow Reply //
    if (1 <= nodeid && nodeid <= 13) {  // Source is a Fellow.
      if (nodeid == b_.cfg_.suid) return;

      Payload msg{};
      memcpy(msg.bytes, packet.payload, payload_size);

      const auto other_suidm1 = nodeid - 1;
      auto& other = roster[other_suidm1];
      other.x = static_cast<double>(msg.fellow_rpl.x);
      other.y = static_cast<double>(msg.fellow_rpl.y);
      other.yaw = static_cast<double>(msg.fellow_rpl.yaw);
      other.since_update_us = 0;

      return;
    } else if (50 <= nodeid && nodeid <= 59) {  // Source is a Commander.
      const auto& om = packet.payload[0];

      ///////////////////
      // B-PPP Command //
      if (om == static_cast<uint8_t>(M::BPPP)) {
        Payload msg{};
        memcpy(msg.bytes, packet.payload, payload_size);

        const auto suidm1 = b_.cfg_.suidm1();
        const auto ppp_idx = msg.cmd.u.bppp.idx[suidm1];
        switch (static_cast<int>(ppp_idx)) {
          // Handle special indices that should be processed or ignored
          // immediately. PPP Command that requests Oneshot should be prevented
          // from changing the Mode of Basilisk.
          case 0:
            return;
          case 50002:
            b_.crmux_ = Basilisk::CRMux::Xbee;
            return;

          // For others, save index and wait to inject.
          default:
            injection_.ppp.idx = ppp_idx;
            injection_.waiting = true;
            return;
        }
      }

      /////////////////////
      // TimeSlottedPoll //
      else if (om == static_cast<uint8_t>(O::TimeSlottedPoll)) {
        b_.poll_clk_us_ = 0;
        return;
      } else {
        return;
      }
    } else {
      return;
    }
  }

  void Inject() {
    b_.cmd_.mode = M::BPPP;
    b_.cmd_.ppp.idx = injection_.ppp.idx;
    b_.cmd_.ppp.prev_mode = b_.cmd_.mode;
  }

  union Payload {
    uint8_t bytes[xb::c::capacity::payload];
    struct __attribute__((packed)) FellowReply {
      float x, y, yaw;
    } fellow_rpl;
    struct __attribute__((packed)) Command {
      uint8_t om;
      union {
        struct __attribute__((packed)) TimeSlottedPoll {
          uint8_t round_robin;
        } tspoll;
        struct __attribute__((packed)) BPPP {
          uint16_t idx[13];
        } bppp;
      } u;
    } cmd;
  };

  struct {
    bool waiting = false;
    struct {
      uint16_t idx = 0;
    } ppp;
  } injection_;

 private:
  Basilisk& b_;
  xb::Receiver r_;
};
