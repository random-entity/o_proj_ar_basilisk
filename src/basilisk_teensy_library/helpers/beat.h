#pragma once

#include <Arduino.h>
#include <elapsedMillis.h>

// Time is in milliseconds, stored as uint32_t.
// Continuously usable up to approximately 50 days.
class Beat {
 public:
  Beat(const uint32_t& interval)
      : next_beat_{millis() + interval + random(interval)},
        interval_{interval} {}

  bool Hit() {
    if (millis() >= next_beat_) {
      while (millis() >= next_beat_) next_beat_ += interval_;
      return true;
    } else {
      return false;
    }
  }

 private:
  uint32_t next_beat_;
  const uint32_t interval_;
};

class elapsedBeat {
 public:
  elapsedBeat(const uint32_t& interval) : interval_{interval} { *this = 0; }

  elapsedBeat& operator=(const uint32_t& val) {
    t_ = val;
    next_beat_ = val;
    return *this;
  }

  bool Hit() {
    if (t_ >= next_beat_) {
      while (t_ >= next_beat_) next_beat_ += interval_;
      return true;
    } else {
      return false;
    }
  }

 private:
  elapsedMillis t_;
  uint32_t next_beat_;
  const uint32_t interval_;
};
