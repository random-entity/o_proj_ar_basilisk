#pragma once

#include <Arduino.h>
#include <elapsedMillis.h>

#include <functional>
#include <map>

#include "consts.h"

namespace xb {

/* Assumes 'API with escapes' as radio operating mode. */
class Receiver {
 public:
  Receiver(HardwareSerial& s,
           const std::function<void(uint8_t* payload, int size)>& callback)
      : s_{s}, callback_{callback} {}

  enum class Waiting {
    Start,
    Length,
    Checksum,
  } w_ = Waiting::Start;

  std::map<Waiting, std::function<void()>> Init = {
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
         size_ = (buf_[0] << 8) + buf_[1];
       }},
  };

  // Call continuously
  void Run() {
    if (w_ == Waiting::Start) {
      while (s_.available() > 0) {
        if (static_cast<uint8_t>(s_.read()) == c::start) {
          Init[Waiting::Length]();
          break;
        }
      }
      if (w_ == Waiting::Start) return;
    }

    if (w_ == Waiting::Length) {
      while (s_.available() > 0) {
        const auto r = static_cast<uint8_t>(s_.read());
        if (r == c::start) {
          Init[Waiting::Length]();
          continue;
        } else {
          if (!Put(r)) {
            /* This should never happen. */
          }
          if (idx_ >= 2) {
            Init[Waiting::Checksum]();
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
          Init[Waiting::Length]();
          break;
        } else {
          if (!Put(r)) {
            /* Receive buffer overflow. */
            Init[Waiting::Start]();
            return;
          }
          if (idx_ >= size_ + 1 /* 1 more due to checksum */) {
            Init[Waiting::Start]();
            callback_(buf_, size_);
            break;
          } else {
            continue;
          }
        }
      }
      if (w_ == Waiting::Checksum) return;
    }
  }

 private:
  bool Put(uint8_t val, bool sum = false) {
    static bool esc = false;
    if (idx_ >= c::buffer_capacity) {
      return false;
    }

    // It is guaranteed that the escape character does not come consecutively.
    if (val == c::escape) {
      esc = true;
      return true;
    }

    if (esc) {
      buf_[idx_++] = val ^ c::xor_with;
    } else {
      buf_[idx_++] = val;
    }
    esc = false;
    return true;
  }

  HardwareSerial& s_;
  uint8_t buf_[c::buffer_capacity];
  int idx_ = 0, size_ = 0, checksum_ = 0;
  std::function<void(uint8_t* payload, int size)> callback_;

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
