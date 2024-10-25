#pragma once

#include <Arduino.h>

#include <map>

namespace g::xb {

namespace addr {
const std::map<uint64_t, int> to_nodeid{
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
                               // {0x0000000000000000, 11},
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
    // The rest are kaktugis
    //.*.*.*.*.*.*.*.*.*.*.*.*.*.
    /////////////////////////////
    {0x0013A20041C2A71B, 14},  // Kaktugi
    /////////////////////////////
    //.*.*.*.*.*.*.*.*.*.*.*.*.*.
    // Goodbye.
    //.*.*.*.*.*.*.*.*.*.*.*.*.*.
    /////////////////////////////
    {((((((0)))))), ((((((0))))))},
};
}  // namespace addr

namespace timing {
/* (all time in milliseconds for this section of comment)
 * Non-Poll Commands: 100M          (M = ?)
 * Polls            : p = 100N + c  (N = span * (0, 1, 2, ...))
 * Replies          : p + e + ((suid + pollid) % 13) * r,
                      skipping around 100K

  Time (ms)        : 0         100       200       300       400
  Non-Poll Cmds    : ?         ?         ?         ?         ?
  Broadcasted Polls: |P        |         |P        |         |P
  Replies          : | 1234567 | 89ABCD  | D123456 | 789ABC  | CD1...
                     <-----span == 2----->
*/

constexpr uint32_t recv_timeout_us = 10 * 1000;  // c
#define R_MINUS_C_US (5 * 1000)
constexpr uint32_t send_interval_us = recv_timeout_us + R_MINUS_C_US;  // r
constexpr uint32_t send_timeout_us = R_MINUS_C_US;
constexpr uint32_t send_init_offset = 5 * 1000;  // e
uint8_t span;
const std::map<int, uint32_t> send_times_us = [] {
  const auto& c = recv_timeout_us;
  const auto& r = send_interval_us;
  const auto& e = send_init_offset;

  std::map<int, uint32_t> result;

  uint8_t i_within_command_interval = 0;
  uint8_t command_interval_idx = 0;

  for (uint8_t i = 0; i <= 12;) {  // i == SUID - 1
    uint32_t base = c + e + command_interval_idx * (100 * 1000);
    uint32_t maybe_next_cmd = (90 + 100 * command_interval_idx) * 1000;
    uint32_t limit = maybe_next_cmd - send_interval_us;
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

}  // namespace timing

}  // namespace g::xb
