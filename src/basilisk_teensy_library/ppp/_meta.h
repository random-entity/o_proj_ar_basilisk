#pragma once

#include <Arduino.h>

#include <functional>
#include <map>

#include "../basilisk.h"
#include "../globals/vars.h"
#include "_index.h"

// * PPP
// ?? ~ ?? Set Base Yaw towards Center and counterclockwise ###TODO###
// 60 ~ 69 Set Pivot Tadak interval ###TODO###
// 70 WalkToPosInField to grid arrangement
// 71 WalkToPosInField to pyramid arrangement
// 72 WalkToPosInField to solo-chorus arrangement
// 73 Sufi to look at solo
// 81 ~ 88 WalkToPosInField to '1-4-8' circular arrangement
// 101 ~ 108 WalkToDir relative to current yaw
// 201 ~ 213 Find SUID
// 701 ~ 713 WalkToPosInField to '0-13' circular arrangement
// 10000 ~ 19999 WalkToPos
// 20000 ~ 29999 WalkToPosInField

namespace ppp {

struct Shooter {
  using M = Basilisk::Command::Mode;
  using O = Basilisk::Command::Oneshots::ByteRep;

  Shooter(Basilisk& _b) : b{_b} {}

  void TibuFoot();
  void TibuIndividual(int mid, AttRel ar);
  void RandomTibutibu();
  void SetGlobalVarSpeed(int level);
  void LookRelativeToCenter(int);
  void Pivot(uint16_t);
  void Sufi(uint16_t);
  void PivSpin(uint16_t);

  void Shoot() {
    if (idx == ppp::idx::idle) {
      m = M::Idle_Init;
    } else if (idx == ppp::idx::free) {
      m = M::Free;
    } else if (idx == ppp::idx::crmux_xbee) {
      // Processed at XbCR.
    } else if (idx == ppp::idx::set_base_yaw_0) {
      b.cmd_.oneshots.Add(O::SetBaseYaw);
      b.cmd_.set_base_yaw.offset = 0.0;
    } else if (idx == ppp::idx::set_base_yaw_m025) {
      b.cmd_.oneshots.Add(O::SetBaseYaw);
      b.cmd_.set_base_yaw.offset = -0.25;
    }

    else if (idx == ppp::range::tibu_foot) {
      TibuFoot();
    } else if (idx == ppp::range::tibu_indiv) {
      int mid = (idx - 5) / 2;
      AttRel ar = idx % 2 ? BOOL_RELEASE : BOOL_ATTACH;
      TibuIndividual(mid, ar);
    } else if (idx == ppp::range::random_tibutibu) {
      RandomTibutibu();
    } else if (idx == ppp::range::set_g_var_speed) {
      SetGlobalVarSpeed(idx - 30);
    } else if (idx == ppp::range::look_rel_to_center) {
      LookRelativeToCenter(idx % 10);
    } else if (idx == ppp::range::bounce_walk) {
      m = M::BounceWalk_Init;
      b.cmd_.bounce_walk.init_tgt_yaw = random(360) / 360.0;
    } else if (idx == ppp::range::pivot) {
      Pivot(idx);
    } else if (idx == ppp::range::sufi) {
      Sufi(idx);
    } else if (idx == ppp::range::piv_spin) {
      PivSpin(idx);
    }
    // ...
  }

  Basilisk& b;
  const uint16_t& idx{b.cmd_.ppp.idx};
  M& m{b.cmd_.mode};
};

}  // namespace ppp
