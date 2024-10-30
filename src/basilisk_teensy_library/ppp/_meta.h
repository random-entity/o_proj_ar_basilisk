#pragma once

#include <Arduino.h>

#include <functional>
#include <map>

#include "../basilisk.h"
#include "../globals/ppp.h"

// * PPP
// ?? ~ ?? Set Base Yaw towards Center and counterclockwise ###TODO###
// 60 ~ 69 Set Pivot Tadak interval ###TODO###
// 70 WalkToPosInField to grid arrangement
// 71 WalkToPosInField to pyramid arrangement
// 72 WalkToPosInField to solo-chorus arrangement
// 73 Sufi to look at solo
// 81 ~ 88 WalkToPosInField to '1-4-8' circular arrangement
// 91 ~ 98 Sufi to target yaw relative to center
// 101 ~ 108 WalkToDir relative to current yaw
// 201 ~ 213 Find SUID
// 701 ~ 713 WalkToPosInField to '0-13' circular arrangement
// 10000 ~ 19999 WalkToPos
// 20000 ~ 29999 WalkToPosInField

namespace ppp {

struct PPP {
  using M = Basilisk::Command::Mode;
  using O = Basilisk::Command::Oneshots::ByteRep;

  PPP(Basilisk& _b) : b{_b} {}

  void Pivot(uint16_t);
  void Sufi(uint16_t);
  void PivSpin(uint16_t);

  void Shoot() {
    if (idx == g::ppp::idx::idle) {
      m = M::Idle_Init;
    } else if (idx == g::ppp::idx::free) {
      m = M::Free;
    } else if (idx == g::ppp::idx::crmux_xbee) {
      // Processed at XbCR.
    } else if (idx == g::ppp::idx::set_base_yaw_0) {
      b.cmd_.oneshots.Add(O::SetBaseYaw);
      b.cmd_.set_base_yaw.offset = 0.0;
    } else if (idx == g::ppp::idx::set_base_yaw_m025) {
      b.cmd_.oneshots.Add(O::SetBaseYaw);
      b.cmd_.set_base_yaw.offset = -0.25;
    }

    else if (idx == g::ppp::range::tibu_foot) {
    } else if (idx == g::ppp::range::pivot) {
      Pivot(idx);
    } else if (idx == g::ppp::range::sufi) {
      Sufi(idx);
    } else if (idx == g::ppp::range::piv_spin) {
      PivSpin(idx);
    }

    // ...
  }

  Basilisk& b;
  const uint16_t& idx{b.cmd_.ppp.idx};
  M& m{b.cmd_.mode};
};
}  // namespace ppp
