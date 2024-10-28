#pragma once

#include <functional>
#include <map>

#include "../basilisk.h"

struct ModeRunners {
  using C = Basilisk::Command;
  using M = C::Mode;

  ModeRunners(Basilisk& _b) : b{_b} {}

  void Idle();
  void Wait();
  void Free();
  void SetMags();
  void SetPhis();

  const std::map<M, std::function<void()>> mode_runners = {
      {M::Idle_Init, [this] { Idle(); }},
      {M::Idle_Nop, [this] { Idle(); }},
      {M::Wait, [this] { Wait(); }},
      {M::Free, [this] { Free(); }},
      {M::SetMags_Init, [this] { SetMags(); }},
      {M::SetMags_Wait, [this] { SetMags(); }},
      {M::SetPhis_Init, [this] { SetPhis(); }},
      {M::SetPhis_Move, [this] { SetPhis(); }},
  };

  Basilisk& b;
  C& c{b.cmd_};
  M& m{b.cmd_.mode};

  // static void BPPP(Basilisk*);
  // static void RandomMags(Basilisk*);
  // static void Pivot(Basilisk*);
  // static void PivSeq(Basilisk*);
  // static void PivSpin(Basilisk*);
  // static void Walk(Basilisk*);
  // static void WalkToDir(Basilisk*);
  // static void WalkToPos(Basilisk*);
  // static void Sufi(Basilisk*);
  // static void Orbit(Basilisk*);
  // static void Diamond(Basilisk*);
  // static void BounceWalk(Basilisk*);
  // static void WalkToPosInField(Basilisk*);
  // static void Shear(Basilisk*);
  // inline static const std::map<M, void (*)(Basilisk*)> mode_runners = {
  //     {M::BPPP, &BPPP},
  //     {M::Free, &Free},
  //     {M::RandomMags_Init, &RandomMags},
  //     {M::RandomMags_Do, &RandomMags},
  //     {M::Pivot_Init, &Pivot},
  //     {M::Pivot_Kick, &Pivot},
  //     {M::PivSeq_Init, &PivSeq},
  //     {M::PivSeq_Step, &PivSeq},
  //     {M::PivSpin, &PivSpin},
  //     {M::Walk, &Walk},
  //     {M::WalkToDir, &WalkToDir},
  //     {M::WalkToPos, &WalkToPos},
  //     {M::Sufi, &Sufi},
  //     {M::Orbit, &Orbit},
  //     {M::Diamond, &Diamond},
  //     {M::BounceWalk_Init, &BounceWalk},
  //     {M::BounceWalk_Reinit, &BounceWalk},
  //     {M::WalkToPosInField_Init, &WalkToPosInField},
  //     {M::WalkToPosInField_Reinit, &WalkToPosInField},
  //     {M::Shear_Init, &Shear},
  //     {M::Shear_Move, &Shear},
  // };
};
