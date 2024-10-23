#pragma once

#include <Arduino.h>

#include "consts.h"

namespace xb {

/* Assumes 'API with escapes' as radio operating mode. */
class Receiver {
 public:
  Receiver(HardwareSerial& s) : s_{s} {}

  void RawPrint() {  // Only for debug. Will block everything.
    while (1) {
      const auto n = s_.available();
      if (n > 0) {
        for (int i = 0; i < s_.available(); i++) {
          Serial.printf("%02X\n", s_.read());
        }
      }
    }
  }

 private:
  HardwareSerial& s_;
};

}  // namespace xb
