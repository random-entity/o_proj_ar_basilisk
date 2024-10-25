#pragma once

#include <Arduino.h>

struct Range {
  Range(uint16_t _from, uint16_t _to) : from{_from}, to{_to} {}

  uint16_t from, to;

  bool operator==(uint16_t val) const { return from <= val && val <= to; }
  friend bool operator==(uint16_t val, const Range& range) {
    return range == val;
  }
};

namespace ppp {
namespace idx {

inline constexpr uint16_t CRMuxXbee = 50002;

const Range tibu{1, 4};
const Range pivot{1000, 2999};
const Range sufi{3300, 3399};
const Range walk_to_pos_in_field{20000, 29999};

}  // namespace idx
}  // namespace ppp
