#pragma once

#include <Arduino.h>

#include <functional>
#include <map>
#include <unordered_set>

#include "../basilisk.h"
#include "../helpers/range.h"

namespace ppp {

namespace idx {

// Oneshots
inline constexpr uint16_t crmux_xbee = 50002;
inline constexpr uint16_t set_base_yaw_0 = 50003;
inline constexpr uint16_t set_base_yaw_m025 = 50004;

// Modes
inline constexpr uint16_t idle = 50000;
inline constexpr uint16_t free = 50001;

}  // namespace idx

namespace range {

const Range<uint16_t> tibu_foot{1, 4};
const Range<uint16_t> tibu_indiv{5, 12};
const Range<uint16_t> random_tibutibu{23, 24};
const Range<uint16_t> set_g_var_speed{30, 34};
const Range<uint16_t> diamond{50, 51};
const Range<uint16_t> look_rel_to_center{91, 98};
const Range<uint16_t> bounce_walk{99, 99};
const Range<uint16_t> pivot{1000, 2999};
const Range<uint16_t> piv_spin{3100, 3299};
const Range<uint16_t> sufi{3300, 3399};
const Range<uint16_t> walk_to_dir{4000, 4999};
const Range<uint16_t> walk_to_pos_in_field{20000, 29999};

}  // namespace range

}  // namespace ppp
