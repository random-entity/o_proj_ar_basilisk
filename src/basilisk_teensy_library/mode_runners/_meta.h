#pragma once

#include <functional>
#include <map>
#include <tuple>
#include <vector>

#include "../basilisk.h"
#include "../globals/vars.h"
#include "../helpers/range.h"

struct ModeRunners {
  using C = Basilisk::Command;
  using M = C::Mode;

  ModeRunners(Basilisk& _b)
      : b{_b},  //
        wt{.c = c.wait},
        mg{.c = c.set_mags},
        rm{.c = c.random_mags},
        ph{.c = c.set_phis},
        pv{.c = c.pivot},
        ps{.c = c.pivseq},
        sp{.c = c.piv_spin},
        wa{.c = c.walk},
        sf{.c = c.sufi},
        wd{.c = c.walk_to_dir},
        wp{.c = c.walk_to_pos},
        bw{.c = c.bounce_walk},
        wf{.c = c.walk_to_pos_in_field} {}

  void Idle();
  void Wait();
  void Free();
  void SetMags();
  void RandomMags();
  void SetPhis();
  void Pivot();
  void PivSeq();
  void PivSpin();
  void Walk();
  void Sufi();
  void WalkToDir();
  void BounceWalk();
  void WalkToPosInField();

  const std::map<M, std::function<void()>> mode_runners = {
      {M::Idle_Init, [this] { Idle(); }},
      {M::Idle_Nop, [this] { Idle(); }},
      {M::Wait, [this] { Wait(); }},
      {M::Free, [this] { Free(); }},
      {M::SetMags_Init, [this] { SetMags(); }},
      {M::SetMags_Wait, [this] { SetMags(); }},
      {M::RandomMags_Init, [this] { RandomMags(); }},
      {M::RandomMags_Do, [this] { RandomMags(); }},
      {M::SetPhis_Init, [this] { SetPhis(); }},
      {M::SetPhis_Move, [this] { SetPhis(); }},
      {M::Pivot_Init, [this] { Pivot(); }},
      {M::Pivot_Kick, [this] { Pivot(); }},
      {M::Pivot_Ta, [this] { Pivot(); }},
      {M::PivSeq_Init, [this] { PivSeq(); }},
      {M::PivSeq_Step, [this] { PivSeq(); }},
      {M::PivSpin, [this] { PivSpin(); }},
      {M::Walk, [this] { Walk(); }},
      {M::Sufi, [this] { Sufi(); }},
      {M::WalkToDir, [this] { WalkToDir(); }},
      {M::BounceWalk_Init, [this] { BounceWalk(); }},
      {M::BounceWalk_Reinit, [this] { BounceWalk(); }},
      {M::WalkToPosInField_Init, [this] { WalkToPosInField(); }},
      {M::WalkToPosInField_Reinit, [this] { WalkToPosInField(); }},
  };

  Basilisk& b;
  C& c{b.cmd_};
  M& m{c.mode};

  struct Wait {
    C::Wait& c;
  } wt;

  struct SetMags {
    C::SetMags& c;
    elapsedMillis since_init;
  } mg;

  struct RandomMags {
    C::RandomMags& c;
    elapsedMillis since_init;
    uint32_t dur[4] = {0};
  } rm;

  struct SetPhis {
    C::SetPhis& c;
    elapsedMillis since_init;
    uint32_t fixing_cycles[2];
  } ph;

  struct Pivot {
    C::Pivot& c;
    int didim_idx;
    int kick_idx;
    elapsedMillis since_init;
    double didim_init_yaw;
    double kick_init_yaw;
    double kick_init_phi_didim;
    elapsedMillis since_ta = 1e9;
  } pv;

  struct PivSeq {
    C::PivSeq& c;
    uint32_t cur_step;
  } ps;

  struct PivSpin {
    C::PivSpin& c;
  } sp;

  struct Walk {
    C::Walk& c;
  } wa;

  struct Sufi {
    C::Sufi& c;
  } sf;

  struct WalkToDir {
    C::WalkToDir& c;
    double init_yaw;
    bool moonwalk;

    // Real target yaw considering moonwalk and nearest_pmn.
    double real_tgt_yaw;
  } wd;

  struct WalkToPos {
    C::WalkToPos& c;
  } wp;

  struct BounceWalk {
    C::BounceWalk& c;
    bool reinit;
    bool trying_overlap_exit[13] = {false};
  } bw;

  struct WalkToPosInField {
    C::WalkToPosInField& c;
    std::pair<Vec2, elapsedMillis> exit_force = {{1e9, 1e9}, 1e9};
    Vec2 stuck_pos{1e9, 1e9};
    double cur_tgt_yaw;
    bool reinit;
  } wf;

  // static void WalkToPos(Basilisk*);
  // static void Orbit(Basilisk*);
  // static void Diamond(Basilisk*);
  // static void Shear(Basilisk*);
  // inline static const std::map<M, void (*)(Basilisk*)> mode_runners = {
  //     {M::WalkToPos, &WalkToPos},
  //     {M::Orbit, &Orbit},
  //     {M::Diamond, &Diamond},
  //     {M::Shear_Init, &Shear},
  //     {M::Shear_Move, &Shear},
  // };
};
