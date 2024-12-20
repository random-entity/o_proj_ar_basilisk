#pragma once

#include <Arduino.h>

#include <functional>
#include <map>

#include "../basilisk.h"
#include "../helpers/range.h"

namespace ppp {

namespace idx {

// Modes
inline constexpr uint16_t idle = 50000;
inline constexpr uint16_t free = 50001;

// Oneshots
inline constexpr uint16_t crmux_xbee = 50002;
inline constexpr uint16_t set_base_yaw_0 = 50003;
inline constexpr uint16_t set_base_yaw_m025 = 50004;
inline constexpr uint16_t save_real_base_yaw = 50005;
inline constexpr uint16_t restore_real_base_yaw = 50006;

}  // namespace idx

namespace range {                                          // Impl Test
const Range<uint16_t> tibu_foot{1, 4};                     // O    O
const Range<uint16_t> tibu_individual{5, 12};              // O    O
const Range<uint16_t> random_tibutibu{23, 24};             // O    O
const Range<uint16_t> set_g_var_speed{30, 34};             // O    O
const Range<uint16_t> diamond{50, 51};                     // X    X
const Range<uint16_t> set_pivot_tadak{60, 69};             // O    O
const Range<uint16_t> arrange_to_grid{70, 70};             // O    X
const Range<uint16_t> arrange_to_pyramid{71, 71};          // O    X
const Range<uint16_t> arrange_to_solo_n_chorus{72, 72};    // O    X
const Range<uint16_t> look_solo{73, 73};                   // O    O
const Range<uint16_t> arrange_to_circle_1_4_8{81, 88};     // O    X
const Range<uint16_t> look_rel_to_center{91, 98};          // O    O
const Range<uint16_t> bounce_walk_random{99, 99};          // O    O
const Range<uint16_t> walk_to_dir_relto_curyaw{101, 108};  // O    O
const Range<uint16_t> bounce_walk_nesw{111, 118};          // O    O
const Range<uint16_t> find_suid{201, 213};                 // X    X
const Range<uint16_t> set_g_var_speed_finer{300, 319};     // O    O
const Range<uint16_t> arrange_to_circle_0_13{701, 713};    // O    X
const Range<uint16_t> pivot{1000, 2999};                   // O    O
const Range<uint16_t> piv_spin{3100, 3299};                // O    O
const Range<uint16_t> sufi{3300, 3399};                    // O    O
const Range<uint16_t> walk_to_dir{4000, 4999};             // O    O
const Range<uint16_t> walk_to_pos{10000, 19999};           // == in_field
const Range<uint16_t> walk_to_pos_in_field{20000, 29999};  // O    O

}  // namespace range

}  // namespace ppp
