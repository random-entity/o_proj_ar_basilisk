#pragma once

#include <Arduino.h>

#include "consts.h"

namespace xb {

/* Assumes 'API with escapes' as radio operating mode. */
class Sender {
 public:
  Sender(HardwareSerial& s) : s_{s} { f_[0] = c::start; }

  bool Send(const uint8_t* frame_data, const int& size,
            const uint64_t& dest_addr = c::addr::broadcast) {
    int idx = 1;
    uint8_t checksum = 0;
    const auto length = size + 14;

    if (!Put(idx, (length >> 8) & 0xFF)) return false;
    if (!Put(idx, length & 0xFF)) return false;
    f_[idx++] = c::frametype::tx_req;
    f_[idx++] = 0x00;  // Suppress response frame
    f_[idx++] = 0x00;  // Broadcast address, MSB
    f_[idx++] = 0x00;
    f_[idx++] = 0x00;
    f_[idx++] = 0x00;
    f_[idx++] = 0x00;
    f_[idx++] = 0x00;
    f_[idx++] = 0xFF;
    f_[idx++] = 0xFF;  // Broadcast address, LSB
    f_[idx++] = 0xFF;  // Unused but typically set to 0xFFFE, MSB
    f_[idx++] = 0xFE;  // Unused but typically set to 0xFFFE, LSB
    f_[idx++] = 0x00;  // Broadcast radius, use NH which must be configured to 1
    f_[idx++] = 0x00;  // Transmit options, use TO which must be configured to
                       // 0x40 (Point-to-multipoint)
    checksum += 11;    // Compensate for not using Put.
    for (int i = 0; i < size; i++) {
      if (!Put(idx, frame_data[i], &checksum)) return false;
    }
    if (!Put(idx, 0xFF - checksum)) return false;

    s_.write(f_, idx);
    return true;
  }

 private:
  bool Put(int& idx, uint8_t val, uint8_t* checksum = nullptr) {
    if (val == c::start || val == c::escape ||  //
        val == c::xon || val == c::xoff) {
      if (idx + 1 >= max_frame_size_) return false;
      f_[idx++] = c::escape;
      f_[idx++] = val ^ c::xor_with;
    } else {
      if (idx >= max_frame_size_) return false;
      f_[idx++] = val;
    }
    if (checksum) *checksum += val;
    return true;
  }

  HardwareSerial& s_;
  inline static constexpr int max_frame_size_ = 50;
  uint8_t f_[max_frame_size_];
};

}  // namespace xb
