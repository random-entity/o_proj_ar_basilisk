#pragma once

#include <Arduino.h>

#include "consts.h"

namespace xb {

/* Assumes 'API with escapes' as radio operating mode. */
class Sender {
 public:
  Sender(HardwareSerial& s) : s_{s} {}

  bool Send(const uint8_t* payload, const int& payload_size,
            const uint64_t& dest_addr = c::addr::broadcast,
            const uint16_t& hima = 0xFFFE) {
    int idx = 1;
    uint8_t sum = 0;
    const auto length = static_cast<uint16_t>(payload_size + 14);

    to_bytes::ui16.n = length;
    for (int b = 1; b >= 0; b--) {
      if (!Put(idx, to_bytes::ui16.bytes[b])) return false;
    }
    if (!Put(idx, c::frametype::txrequest, &sum)) return false;
    if (!Put(idx, 0)) return false;  // Suppress response frame.
    to_bytes::ui64.n = dest_addr;
    for (int b = 7; b >= 0; b--) {
      if (!Put(idx, to_bytes::ui64.bytes[b], &sum)) return false;
    }
    to_bytes::ui16.n = hima;
    for (int b = 1; b >= 0; b--) {
      if (!Put(idx, to_bytes::ui16.bytes[b], &sum)) return false;
    }
    if (!Put(idx, 0)) return false;  // Use NH value for broadcast radius.
    if (!Put(idx, 0)) return false;  // Use TO value for transmit options.
    for (int i = 0; i < payload_size; i++) {
      if (!Put(idx, payload[i], &sum)) return false;
    }
    if (!Put(idx, 0xFF - sum)) return false;
    s_.write(buf_, idx);
    return true;
  }

 private:
  bool Put(int& idx, uint8_t val, uint8_t* sum = nullptr) {
    if (val == c::start || val == c::escape ||  //
        val == c::xon || val == c::xoff) {
      if (idx + 1 >= c::buffer_capacity) return false;
      buf_[idx++] = c::escape;
      buf_[idx++] = val ^ c::xor_with;
    } else {
      if (idx >= c::buffer_capacity) return false;
      buf_[idx++] = val;
    }
    if (sum) *sum += val;
    return true;
  }

  HardwareSerial& s_;
  uint8_t buf_[c::buffer_capacity] = {c::start};

  struct to_bytes {  // Little endian.
    inline static union {
      uint64_t n;
      uint8_t bytes[8];
    } ui64;

    inline static union {
      uint16_t n;
      uint8_t bytes[2];
    } ui16;
  };
};

}  // namespace xb
