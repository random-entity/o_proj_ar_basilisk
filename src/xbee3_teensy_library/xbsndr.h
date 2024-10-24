#pragma once

#include <Arduino.h>

#include "consts.h"

namespace xb {

/* Assumes 'API with escapes' as radio operating mode. */
class Sender {
 public:
  Sender(HardwareSerial& s) : s_{s} {}

  bool Send(const uint8_t* frame_data, const int& size,
            const uint64_t& dest_addr = c::addr::broadcast) {
    int idx = 1;
    uint8_t checksum = 0;
    const auto length = size + 14;

    if (!Put(idx, (length >> 8) & 0xFF)) return false;
    if (!Put(idx, length & 0xFF)) return false;
    buf_[idx++] = c::frametype::txreq;
    buf_[idx++] = 0x00;  // Suppress response frame
    buf_[idx++] = 0x00;  // Broadcast address, MSB
    buf_[idx++] = 0x00;
    buf_[idx++] = 0x00;
    buf_[idx++] = 0x00;
    buf_[idx++] = 0x00;
    buf_[idx++] = 0x00;
    buf_[idx++] = 0xFF;
    buf_[idx++] = 0xFF;  // Broadcast address, LSB
    buf_[idx++] = 0xFF;  // Unused but typically set to 0xFFFE, MSB
    buf_[idx++] = 0xFE;  // Unused but typically set to 0xFFFE, LSB
    buf_[idx++] = 0x00;  // Broadcast radius, use NH, configured to 1
    buf_[idx++] = 0x00;  // Transmit options, use TO, configured to 0x40
    checksum += 11;      // Compensate for not using Put.
    for (int i = 0; i < size; i++) {
      if (!Put(idx, frame_data[i], &checksum)) return false;
    }
    if (!Put(idx, 0xFF - checksum)) return false;

    s_.write(buf_, idx);
    return true;
  }

 private:
  bool Put(int& idx, uint8_t val, uint8_t* checksum = nullptr) {
    if (val == c::start || val == c::escape ||  //
        val == c::xon || val == c::xoff) {
      if (idx + 1 >= c::buffer_capacity) return false;
      buf_[idx++] = c::escape;
      buf_[idx++] = val ^ c::xor_with;
    } else {
      if (idx >= c::buffer_capacity) return false;
      buf_[idx++] = val;
    }
    if (checksum) *checksum += val;
    return true;
  }

  HardwareSerial& s_;
  uint8_t buf_[c::buffer_capacity] = {c::start};
};

}  // namespace xb
