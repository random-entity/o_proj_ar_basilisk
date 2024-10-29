#pragma once

#include <Arduino.h>

namespace xb::c {

inline constexpr uint8_t start = 0x7E;
inline constexpr uint8_t escape = 0x7D;
inline constexpr uint8_t xon = 0x11;
inline constexpr uint8_t xoff = 0x13;
inline constexpr uint8_t xor_with = 0x20;

namespace capacity {
inline constexpr uint16_t buffer = 100;
inline constexpr uint16_t tx_payload = (buffer - 1) / 2 - 17;
}  // namespace capacity

namespace frametype {
inline constexpr uint8_t tx = 0x10;
inline constexpr uint8_t rx = 0x90;
}  // namespace frametype

namespace addr {
inline constexpr uint64_t broadcast = 0xFFFF;
}  // namespace addr

}  // namespace xb::c
