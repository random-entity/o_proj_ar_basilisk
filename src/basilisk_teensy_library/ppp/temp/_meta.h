#pragma once

#include "../cmd_rcvrs/xbee_cr.h"
#include "../basilisk.h"

namespace presets::globalvar {
PhiSpeed speed;
}

struct Presets {
  using M = Basilisk::Command::Mode;

  static void RMagRls(Basilisk*);
  static void RMagAtt(Basilisk*);
  static void LMagRls(Basilisk*);
  static void LMagAtt(Basilisk*);
  static void RandomMagsWeak(Basilisk*);
  static void RandomMagsStrong(Basilisk*);
  static void SetGlobalSetPhisSpeed(Basilisk*, int);
  inline static void Diamond(Basilisk* b, LR init_didimbal) {
    auto& m = b->cmd_.mode;
    auto& c = b->cmd_.diamond;

    m = M::Diamond;
    c.init_didimbal = init_didimbal;
    c.init_stride = 0.3;
    c.speed = globals::stdval::speed::normal;
    c.acclim = globals::stdval::acclim::standard;
    c.min_stepdur = 0;
    c.max_stepdur = -1;
    c.interval = 0;
    c.steps = -1;
  }
  inline static void RandomBounceWalk(Basilisk* b) {
    auto& m = b->cmd_.mode;
    auto& c = b->cmd_.bounce_walk;

    m = M::BounceWalk_Init;
    c.init_tgt_yaw = random(360) / 360.0;
  }

  inline static const std::map<uint16_t, void (*)(Basilisk*)> presets = {
      // Specific
      {1, &RMagRls},
      {2, &RMagAtt},
      {3, &LMagRls},
      {4, &LMagAtt},
      {23, &RandomMagsWeak},
      {24, &RandomMagsStrong},
      {30, [](Basilisk* b) { SetGlobalSetPhisSpeed(b, 0); }},
      {31, [](Basilisk* b) { SetGlobalSetPhisSpeed(b, 1); }},
      {32, [](Basilisk* b) { SetGlobalSetPhisSpeed(b, 2); }},
      {33, [](Basilisk* b) { SetGlobalSetPhisSpeed(b, 3); }},
      {34, [](Basilisk* b) { SetGlobalSetPhisSpeed(b, 4); }},
      {50, [](Basilisk* b) { Diamond(b, BOOL_L); }},
      {51, [](Basilisk* b) { Diamond(b, BOOL_R); }},
      {99, &RandomBounceWalk},
  };
};