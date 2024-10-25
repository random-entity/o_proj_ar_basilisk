#pragma once

#include <Arduino.h>

#include "../helpers/range.h"

namespace g ::ppp {
namespace idx {

inline constexpr uint16_t crmux_xbee = 50002;

const Range<uint16_t> tibu{1, 4};
const Range<uint16_t> pivot{1000, 2999};
const Range<uint16_t> sufi{3300, 3399};
const Range<uint16_t> walk_to_pos_in_field{20000, 29999};

}  // namespace idx
}  // namespace g::ppp
