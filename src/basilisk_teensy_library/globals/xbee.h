#pragma once

#include <Arduino.h>

#include <map>

namespace g::xb {

namespace addr {

/* 64-bit MAC address -> Node ID within XBee network */
const std::map<uint64_t, int> to_nid{
    /////////////////////////////
    //.*.*.*.*.*.*.*.*.*.*.*.*.*.
    // 1 <= Followers <= 13
    //.*.*.*.*.*.*.*.*.*.*.*.*.*.
    /////////////////////////////
    {0x0013A20041C22F0E, 1},   //
    {0x0013A20041C22D2A, 2},   //
    {0x0013A20041C2AE4E, 3},   //
    {0x0013A20041C2AF68, 4},   //
    {0x0013A20041C15729, 5},   //
    {0x0013A20041C22D41, 6},   //
    {0x0013A20041C1369C, 7},   //
    {0x0013A20041C2B6D4, 8},   //
    {0x0013A20041C22F71, 9},   //
    {0x0013A20041BEA1C1, 10},  //
    {0x0013A20041C22D99, 11},  //
    {0x0013A20041C22EFC, 12},  //
    {0x0013A20041C2F781, 13},  //
    /////////////////////////////
    //.*.*.*.*.*.*.*.*.*.*.*.*.*.
    // 50 <= Commanders <= 59
    //.*.*.*.*.*.*.*.*.*.*.*.*.*.
    /////////////////////////////
    {0x0013A20041C2E64D, 50},  // Commander (Pd)
    {0x0013A20041C14EC5, 51},  // Monitor (Processing)
    /////////////////////////////
    //.*.*.*.*.*.*.*.*.*.*.*.*.*.
    // The rest are kaktugis.
    // Goodbye.
    //.*.*.*.*.*.*.*.*.*.*.*.*.*.
    /////////////////////////////
    {0x0013A20041C2A71B, 14},  // Kaktugi
};

}  // namespace addr

/* (all time in milliseconds for this section of comment)
 * Non-Poll Commands : 100M      (M = ?)
 * Time Slotted Polls: 100N + p  (N = span * (0, 1, 2, ...))
 * Replies           : 100N + (f = p + e) + ((suid + robin) % 13) * r,
 *                     skipping around 100K
 *
 * Time (ms)        : 0         100       200       300       400
 * Non-Poll Cmds    : ?         ?         ?         ?         ?
 * Broadcasted Polls: |P        |         |P        |         |P
 * Replies          : | 1234567 | 89ABCD  | D123456 | 789ABC  | CD1...
 *                    <-----span == 2----->                         */
struct Timing {
  inline static constexpr uint32_t p_us = 10 * 1000;  // p
  inline static constexpr uint32_t f_us = 15 * 1000;  // f = p + e
  inline static constexpr uint32_t r_us = 10 * 1000;  // r
  inline static constexpr uint32_t send_timeout_us = r_us * 3 / 4;

  static int span;

  inline static const std::map<int, uint32_t> send_times_us = [] {
    const auto& r = r_us;
    const auto& e = f_us;

    std::map<int, uint32_t> result;

    uint8_t i_within_command_interval = 0;
    uint8_t command_interval_idx = 0;

    for (uint8_t i = 0; i <= 12;) {  // i == SUID - 1
      uint32_t base = p_us + e + command_interval_idx * (100 * 1000);
      uint32_t maybe_next_cmd = (90 + 100 * command_interval_idx) * 1000;
      uint32_t limit = maybe_next_cmd - r_us;
      uint32_t maybe_send_time = base + i_within_command_interval * r;

      if (maybe_send_time <= limit) {
        result[i + 1] = maybe_send_time;  // TODO: Change to [0, 12] -> time
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
};

}  // namespace g::xb
