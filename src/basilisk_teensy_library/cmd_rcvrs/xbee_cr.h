#pragma once

#include <Xbee3.h>

#include "../basilisk.h"
#include "../globals/serials.h"
#include "../globals/xbee.h"
#include "../roster.h"

/* This class is capable of handling the following type of messages:
 * - Broadcasted parameterized-preset-protocol (B-PPP) Commands
 *   <- Recognized iff (source is Commander) && payload[O/M] == 4.
 *   -> Set waiting injection flag and inject at next ExecutionCycle.
 * - Broadcasted Poll Commands
 *   <- Recognized iff (source is Commander) && payload[O/M] == 253.
 *   -> Reserve time slot for next ReplySend.
 * - Fellow Replies
 *   <- Recognized iff (source is Fellow).
 *   -> Save to Roster immediately. */
class XbeeCommandReceiver {
  using C = Basilisk::Command;
  using M = C::Mode;
  using O = C::Oneshot;

 public:
  XbeeCommandReceiver(Basilisk& b)
      : b_{b},
        r_{XBEE_SERIAL, [this](xb::ReceivePacket& packet, int payload_size) {
             Parse(packet, payload_size);
           }} {}

  // Call continuously.
  void Run() { r_.Run(); }

  void Parse(xb::ReceivePacket& packet, int payload_size) {
    const auto maybe_nodeid_it = xb::addr::to_nodeid.find(packet.src_addr());
    if (maybe_nodeid_it == xb::addr::to_nodeid.end()) return;
    const auto nodeid = maybe_nodeid_it->second;
    if (1 <= nodeid && nodeid <= 13) {  // Fellow Reply
      if (nodeid == b_.cfg_.suid) return;

    } else if (50 <= nodeid && nodeid <= 59) {  // Source is a Commander
      const auto& om = packet.payload[0];
      if (om == static_cast<uint8_t>(M::BPPP)) {  // B-PPP Command
      } else if (om == static_cast<uint8_t>(
                           O::BroadcastedPoll)) {  // Broadcasted Poll Command
      } else {
        return;
      }
    } else {
      return;
    }
  }

  union Message {
    uint8_t payload[ xb::c:: ];

  } injection_;


  struct __attribute__((packed)) Message {
    union {}



    uint8_t om;
    union {
      struct __attribute__((packed))
      {
        /* data */
      } bppp;
      
      struct __attribute__((packed)) {
        float x, y, yaw;
      } fellow_rpl;
    } u;
  } ;

 private:
  Basilisk& b_;
  xb::Receiver r_;
};
