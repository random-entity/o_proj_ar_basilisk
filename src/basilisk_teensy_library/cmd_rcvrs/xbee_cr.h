#pragma once

#include <Xbee3.h>

#include "../basilisk.h"
#include "../globals/ppp.h"
#include "../globals/serials.h"
#include "../globals/xbee.h"
#include "../roster.h"

/* This class is capable of handling the following type of messages:
 * - Broadcasted parameterized-preset-protocol (B-PPP) Commands:
 *   <- Is iff (source is Commander) && payload[O/M] == M::BPPP.
 *   -> Set waiting injection flag to inject at next ExecutionCycle.
 *     * Waiting is for time synchronization with ModeRunners.
 * - Broadcasted Poll Commands:
 *   <- Is iff (source is Commander) && payload[O/M] == O::BroadcastedPoll.
 *   -> Reset bpoll clock for next ReplySend.
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
        r_{g::serials::xb, [this](xb::ReceivePacket& packet, int payload_size) {
             Parse(packet, payload_size);
           }} {
#if DEBUG_SETUP
    Pln("XbeeCommandReceiver: Setup complete");
#endif
  }

  // Call continuously.
  void Run() { r_.Run(); }

  void Parse(xb::ReceivePacket& packet, int payload_size) {
    const auto srcnid_it = g::xb::addr::to_nid.find(packet.src_addr());
    if (srcnid_it == g::xb::addr::to_nid.end()) return;
    const auto srcnid = srcnid_it->second;

    //////////////////
    // Fellow Reply //
    if (1 <= srcnid && srcnid <= 13) {  // Source is a Fellow.
      if (srcnid == b_.cfg_.suid) return;

      Payload msg{};
      memcpy(msg.bytes, packet.payload, payload_size);

      const auto other_suidm1 = srcnid - 1;
      auto& other = roster[other_suidm1];
      other.x = static_cast<double>(msg.fellow_rpl.x);
      other.y = static_cast<double>(msg.fellow_rpl.y);
      other.yaw = static_cast<double>(msg.fellow_rpl.yaw);
      other.since_update_us = 0;

      return;
    } else if (50 <= srcnid && srcnid <= 59) {  // Source is a Commander.
      const auto& om = packet.payload[0];

      ///////////////////
      // B-PPP Command //
      if (om == static_cast<uint8_t>(M::BPPP)) {
        Payload msg{};
        memcpy(msg.bytes, packet.payload, payload_size);

        const auto suidm1 = b_.cfg_.suidm1();
        const auto ppp_idx = msg.cmd.u.bppp.idx[suidm1];
        switch (ppp_idx) {
          // Handle special indices that should be processed or ignored
          // immediately. PPP Command that requests Oneshot should be prevented
          // from changing the Mode of Basilisk.
          case 0:
            return;
          case g::ppp::idx::crmux_xbee:
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
      // BroadcastedPoll //
      else if (om == static_cast<uint8_t>(O::BroadcastedPoll)) {
        b_.since_bpoll_us_ = 0;
        Payload msg{};
        memcpy(msg.bytes, packet.payload, payload_size);
        b_.cmd_.bpoll.round_robin = msg.cmd.u.bpoll.round_robin;
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
        struct __attribute__((packed)) BroadcastedPoll {
          uint8_t round_robin;
        } bpoll;
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
