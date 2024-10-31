#pragma once

#include <Arduino.h>

#include <functional>
#include <map>

#include "../basilisk.h"
#include "../globals/vars.h"
#include "_index.h"

struct PPPShooter {
  using M = Basilisk::Command::Mode;
  using O = Basilisk::Command::Oneshots::ByteRep;

  PPPShooter(Basilisk& _b) : b{_b} {}

  // mags.h
  void TibuFoot();
  void TibuIndividual(int mid, AttRel ar);
  void RandomTibutibu();

  // set_g_var.h
  void SetGlobalVarSpeed(int level);
  void SetGlobalVarSpeedFiner(int level);

  // arrange.h
  void ArrangeToGrid();
  void ArrangeToPyramid();
  void ArrangeToSoloNChorus();
  void ArrangeToCircle148(int senw);
  void ArrangeToCircle013(int robin);

  // look.h
  void LookSolo();
  void LookRelativeToCenter(int);

  void Pivot(uint16_t);
  void Sufi(uint16_t);
  void PivSpin(uint16_t);

  // walks.h
  void WalkToDirRelToCurYaw(int senw);
  void WalkToDir(int last_3_digits);
  void WalkToPosInField(int last_4_digits);

  void Shoot() {
    if (idx == ppp::idx::idle) {
      m = M::Idle_Init;
    }

    else if (idx == ppp::idx::free) {
      m = M::Free;
    }

    else if (idx == ppp::idx::crmux_xbee) {
      // Processed at XbCR.
    }

    else if (idx == ppp::idx::set_base_yaw_0) {
      b.cmd_.oneshots.Add(O::SetBaseYaw);
      b.cmd_.set_base_yaw.offset = 0.0;
    }

    else if (idx == ppp::idx::set_base_yaw_m025) {
      b.cmd_.oneshots.Add(O::SetBaseYaw);
      b.cmd_.set_base_yaw.offset = -0.25;
    }

    else if (idx == ppp::idx::save_real_base_yaw) {
      b.cmd_.oneshots.Add(O::SaveRealBaseYaw);
    }

    else if (idx == ppp::idx::restore_real_base_yaw) {
      b.cmd_.oneshots.Add(O::RestoreRealBaseYaw);
    }

    else if (idx == ppp::range::tibu_foot) {
      TibuFoot();
    }

    else if (idx == ppp::range::tibu_individual) {
      int mid = (idx - 5) / 2;
      AttRel ar = idx % 2 ? BOOL_RELEASE : BOOL_ATTACH;
      TibuIndividual(mid, ar);
    }

    else if (idx == ppp::range::random_tibutibu) {
      RandomTibutibu();
    }

    else if (idx == ppp::range::set_g_var_speed) {
      SetGlobalVarSpeed(idx - 30);
    }

    else if (idx == ppp::range::diamond) {
      // Not implemented.
    }

    else if (idx == ppp::range::set_pivot_tadak) {
      g::vars::pivot_tadak = 200 * (idx % 10);
    }

    else if (idx == ppp::range::arrange_to_grid) {
      ArrangeToGrid();
    }

    else if (idx == ppp::range::arrange_to_pyramid) {
      ArrangeToPyramid();
    }

    else if (idx == ppp::range::arrange_to_solo_n_chorus) {
      ArrangeToSoloNChorus();
    }

    else if (idx == ppp::range::look_solo) {
      LookSolo();
    }

    else if (idx == ppp::range::arrange_to_circle_1_4_8) {
      ArrangeToCircle148(idx % 10);
    }

    else if (idx == ppp::range::look_rel_to_center) {
      LookRelativeToCenter(idx % 10);
    }

    else if (idx == ppp::range::bounce_walk_random) {
      m = M::BounceWalk_Init;
      b.cmd_.bounce_walk.init_tgt_yaw = random(360) / 360.0;
    }

    else if (idx == ppp::range::walk_to_dir_relto_curyaw) {
      WalkToDirRelToCurYaw(idx % 10);
    }

    else if (idx == ppp::range::bounce_walk_nesw) {
      m = M::BounceWalk_Init;
      int nesw = idx % 10;
      b.cmd_.bounce_walk.init_tgt_yaw = (nesw - 3) * 0.125;
    }

    else if (idx == ppp::range::find_suid) {
      // Not implemented.
    }

    else if (idx == ppp::range::set_g_var_speed_finer) {
      SetGlobalVarSpeedFiner(static_cast<int>(idx) - 300);
    }

    else if (idx == ppp::range::arrange_to_circle_0_13) {
      ArrangeToCircle013(idx - 700);
    }

    else if (idx == ppp::range::pivot) {
      Pivot(idx);
    }

    else if (idx == ppp::range::piv_spin) {
      PivSpin(idx);
    }

    else if (idx == ppp::range::sufi) {
      Sufi(idx);
    }

    else if (idx == ppp::range::walk_to_dir) {
      WalkToDir(static_cast<int>(idx) % 1000);
    }

    else if (idx == ppp::range::walk_to_pos) {
      WalkToPosInField(static_cast<int>(idx) % 10000);
    }

    else if (idx == ppp::range::walk_to_pos_in_field) {
      WalkToPosInField(static_cast<int>(idx) % 10000);
    }

    // ...
  }

  Basilisk& b;
  const uint16_t& idx{b.cmd_.ppp.idx};
  M& m{b.cmd_.mode};
};
