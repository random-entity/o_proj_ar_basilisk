#pragma once

#include <Xbee3.h>

#include "../basilisk.h"
#include "../globals/serials.h"
#include "../globals/xbee.h"
#include "../ppp/_index.h"
#include "../roster.h"

/* This class is capable of handling the following type of messages:
 * - Broadcasted parameterized-preset-protocol (B-PPP) Oneshots:
 *   <- Is iff (source is Commander) && payload[0] == byterep(B-PPP).
 *   -> Set oneshot for next ExecutionCycle if CRMux is Xbee.
 *     * Waiting for next ExecutionCycle is for synchronization with ModeRunner.
 * - Broadcasted Poll Oneshots:
 *   <- Is iff (source is Commander) && payload[0] == byterep(BroadcastedPoll).
 *   -> Immediately reset bpoll clock for next ReplySend.
 * - Fellow Replies:
 *   <- Is iff (source is Fellow).
 *   -> Immediately save to Roster. */
class XbeeCommandReceiver {
  using O = Basilisk::Command::Oneshots::ByteRep;

 public:
  XbeeCommandReceiver(Basilisk& b)
      : b_{b},
        r_{g::serials::xb,
           [this](xb::ReceivePacket& packet, uint16_t payload_size) {
             Parse(packet, payload_size);
           }} {
#if DEBUG_SETUP
    Pln("XbeeCommandReceiver: Setup complete");
#endif
  }

  // Call continuously.
  void Run() { r_.Run(); }

  void Parse(xb::ReceivePacket& packet, uint16_t payload_size) {
    const auto srcnid_it = g::xb::addr::to_nid.find(packet.src_addr());
    if (srcnid_it == g::xb::addr::to_nid.end()) return;
    const auto srcnid = srcnid_it->second;

#if DEBUG_XBEE
    P("XbCR: RX payload -> ");
    for (auto i = 0; i < payload_size; i++) {
      Serial.print(packet.payload[i]);
      P(", ");
    }
    Serial.println();
#endif

    //////////////////
    // Fellow Reply //
    if (srcnid == g::xb::nid::range::followers) {  // Source is a Fellow.
      if (srcnid == b_.cfg_.suid) return;
#if DEBUG_XBEE
      Pln("XbCR: Received FellowReply");
#endif

      Payload msg{};
      memcpy(msg.bytes, packet.payload, payload_size);

      const auto other_suidm1 = srcnid - 1;
      auto& other = roster[other_suidm1];
      other.x = static_cast<double>(msg.fellow_rpl.lpsx);
      other.y = static_cast<double>(msg.fellow_rpl.lpsy);
      other.yaw = static_cast<double>(msg.fellow_rpl.yaw);
      other.since_update_us = 0;

      return;
    }

    if (srcnid == g::xb::nid::range::commanders) {
      // Source is a Commander.
      const auto& mob = packet.payload[0];

      /////////////////////
      // BroadcastedPoll //
      if (mob == Basilisk::Command::ByteRepRanges::bpoll) {
#if DEBUG_XBEE
        Pln("XbCR: Received BPoll");
#endif

        b_.cmd_.bpoll.since_us = 0;
        Payload msg{};
        memcpy(msg.bytes, packet.payload, payload_size);
        b_.cmd_.bpoll.round_robin = msg.cmd.u.bpoll.round_robin;
        return;
      }

      ///////////////////
      // B-PPP Oneshot //
      if (mob == static_cast<uint8_t>(O::BPPP)) {
#if DEBUG_XBEE
        Pln("XbCR: Received BPPP");
#endif

        Payload msg{};
        memcpy(msg.bytes, packet.payload, payload_size);

        const auto& suidm1 = b_.cfg_.suidm1;
        const auto& ppp_idx = msg.cmd.u.bppp.idx[suidm1];

        b_.rpl_.since_xbrx_us.bppp = 0;

        switch (ppp_idx) {
          // Handle special indices that should be processed or ignored
          // immediately.
          case 0:
            // Ignore PPP index 0.
            return;
          case g::ppp::idx::crmux_xbee:
            b_.crmux_ = Basilisk::CRMux::Xbee;
            return;

          // For others, set oneshot for next ExecutionCycle if CRMux is Xbee.
          default:
            if (b_.crmux_ == Basilisk::CRMux::Xbee) {
              b_.cmd_.oneshots.Add(O::BPPP);
              b_.cmd_.ppp.idx = ppp_idx;
            }
            return;
        }
      }

      return;
    }
  }

  union Payload {
    uint8_t bytes[xb::c::capacity::tx_payload];
    struct __attribute__((packed)) FellowReply {
      float phi_l, phi_r, lpsx, lpsy, yaw;
    } fellow_rpl;
    struct __attribute__((packed)) Command {
      uint8_t mob;
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

 private:
  Basilisk& b_;
  xb::Receiver r_;
};
