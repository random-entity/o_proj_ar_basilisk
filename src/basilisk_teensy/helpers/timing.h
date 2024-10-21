#pragma once

#include <Arduino.h>

#include <map>

namespace timing::xb {

// Non-Poll Commands: (100ms)M  (M = ?)
// Polls            : p = (100ms)N + c  (N = 0, s, 2s, ...)
// Replies          : p + e + (suid - 1) * r, skipping around (100ms)M

const uint32_t receive_timeout_us = 10 * 1000;  // c
#define R_MINUS_C_US (5 * 1000)
const uint32_t send_interval_us = receive_timeout_us + R_MINUS_C_US;  // r
const uint32_t send_timeout_us = R_MINUS_C_US;
const uint32_t send_init_offset = 5 * 1000;  // e
uint8_t span;
const std::map<uint8_t, uint32_t> suid_to_send_time_us = [] {
  const auto& c = receive_timeout_us;
  const auto& r = send_interval_us;
  const auto& e = send_init_offset;

  std::map<uint8_t, uint32_t> result;

  uint8_t i_within_command_interval = 0;
  uint8_t command_interval_idx = 0;

  for (uint8_t i = 0; i <= 12;) {  // i == SUID - 1
    uint32_t base = c + e + command_interval_idx * (100 * 1000);
    uint32_t maybe_next_cmd = (90 + 100 * command_interval_idx) * 1000;
    uint32_t limit = maybe_next_cmd - send_interval_us;
    uint32_t maybe_send_time = base + i_within_command_interval * r;

    if (maybe_send_time <= limit) {
      result[i + 1] = maybe_send_time;
      i++;
      i_within_command_interval++;
    } else {
      command_interval_idx++;
      i_within_command_interval = 0;
    }
  }

  span = command_interval_idx + 1;

  return result;
}();

}  // namespace timing::xb
