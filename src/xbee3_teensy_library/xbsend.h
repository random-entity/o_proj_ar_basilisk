#pragma once

#include <Arduino.h>

#include "consts.h"

namespace xb {

/* Assumes 'API with escapes' as radio operating mode. */
class Sender {
 public:
  Sender(HardwareSerial& s) : s_{s} {}

  void Send(const uint8_t* frame_data, const uint32_t& len,
            const uint64_t& dest_addr = c::addr::broadcast) {
    int idx = 1;
    f_[idx++] = (len >> 8) & 0xFF;
    f_[idx++] = len & 0xFF;

    s_.write(f_, 3);
  }

 private:
  HardwareSerial& s_;
  uint8_t f_[32] = {c::start};
};

}  // namespace xb
