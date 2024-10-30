#pragma once

#include <Arduino.h>

#include <functional>
#include <map>

#include "consts.h"

namespace xb {

// Not including Start Delimiter and Length bytes.
struct ReceivePacket {
  uint8_t frame_type;          // 0x90 for Receive Packet.
  uint64_t src_addr_reversed;  // Who sent this, big endian, so byte-reversed.
  uint16_t random_entity;      // The meaningless zone.
  uint8_t recv_options;        // 0x42 for Point-multipoint broadcast.
  uint8_t payload[c::capacity::buffer - 13];

  uint64_t src_addr() {
    const auto& r = src_addr_reversed;
    return                                                         //
        (static_cast<uint64_t>(r & 0xFFULL) << 56) |               //
        (static_cast<uint64_t>(r & 0xFF00ULL) << 40) |             //
        (static_cast<uint64_t>(r & 0xFF0000ULL) << 24) |           //
        (static_cast<uint64_t>(r & 0xFF000000ULL) << 8) |          //
        (static_cast<uint64_t>(r & 0xFF00000000ULL) >> 8) |        //
        (static_cast<uint64_t>(r & 0xFF0000000000ULL) >> 24) |     //
        (static_cast<uint64_t>(r & 0xFF000000000000ULL) >> 40) |   //
        (static_cast<uint64_t>(r & 0xFF00000000000000ULL) >> 56);  //
  }
} __attribute__((packed));

/* Assumes 'API with escapes' as radio operating mode. */
class Receiver {
 public:
  Receiver(HardwareSerial& s,
           const std::function<void(ReceivePacket&, uint16_t)>& callback)
      : s_{s}, callback_{callback} {
    // It is the client's responsibility to begin the HardwareSerial.
  }

 private:
  enum class Waiting {
    Start,
    Length,
    Checksum,
  } w_ = Waiting::Start;

  const std::map<Waiting, std::function<void()>> go_to_ = {
      {Waiting::Start,
       [&] {  //
         w_ = Waiting::Start;
       }},
      {Waiting::Length,
       [&] {
         w_ = Waiting::Length;
         idx_ = 0;
       }},
      {Waiting::Checksum,
       [&] {
         w_ = Waiting::Checksum;
         idx_ = 0;
         checksum_ = 0;
         size_ = (buf_.bytes[0] << 8) + buf_.bytes[1];
       }},
  };

 public:
  // Call continuously.
  void Run() {
    if (w_ == Waiting::Start) {
      while (s_.available() > 0) {
        if (static_cast<uint8_t>(s_.read()) == c::start) {
          go_to_.at(Waiting::Length)();
          break;
        }
      }
      if (w_ == Waiting::Start) return;
    }

    if (w_ == Waiting::Length) {
      while (s_.available() > 0) {
        const auto r = static_cast<uint8_t>(s_.read());
        if (r == c::start) {
          go_to_.at(Waiting::Length)();
          continue;
        } else {
          if (!Put(r)) {
            /* This should never happen if c::capacity::buffer >= 4. */
            go_to_.at(Waiting::Start)();
            return;
          }
          if (idx_ >= 2) {
            go_to_.at(Waiting::Checksum)();
            if (size_ < 12) {
              /* This should never happen if XBee is sane. */
              go_to_.at(Waiting::Start)();
              return;
            }
            break;
          } else {
            continue;
          }
        }
      }
      if (w_ == Waiting::Length) return;
    }

    if (w_ == Waiting::Checksum) {
      while (s_.available() > 0) {
        const auto r = static_cast<uint8_t>(s_.read());
        if (r == c::start) {
          go_to_.at(Waiting::Length)();
          return;
        } else {
          if (!Put(r, true)) {
            /* Receive buffer overflow. */
            go_to_.at(Waiting::Start)();
            return;
          }
          if (idx_ >= size_ + 1 /* One more byte due to the checksum. */) {
            if (checksum_ == 0xFF &&
                buf_.packet.frame_type == c::frametype::rx) {
              callback_(buf_.packet, size_ - 12);
            }
            go_to_.at(Waiting::Start)();
            return;
          } else {
            continue;
          }
        }
      }
    }
  }

 private:
  bool Put(uint8_t val, bool sum = false) {
    if (idx_ >= c::capacity::buffer) {
      return false;
    }

    // It is guaranteed that the escape character does not come consecutively.
    if (val == c::escape) {
      esc_ = true;
      return true;
    }

    val = esc_ ? val ^ c::xor_with : val;
    buf_.bytes[idx_++] = val;
    if (sum) checksum_ += val;
    esc_ = false;
    return true;
  }

  HardwareSerial& s_;
  union {
    uint8_t bytes[c::capacity::buffer];
    ReceivePacket packet;
  } buf_;
  uint8_t idx_ = 0;
  uint16_t size_ = 0;
  uint8_t checksum_ = 0;
  bool esc_ = false;
  std::function<void(ReceivePacket& packet, uint16_t payload_size)> callback_;

 public:
  void RawPrint() {  // Only for debug. Blocks everything.
    while (1) {
      const auto n = s_.available();
      if (n > 0) {
        for (int i = 0; i < n; i++) {
          Serial.printf("%02X\n", s_.read());
        }
      }
    }
  }
};

}  // namespace xb
