#pragma once

#include "_meta.h"

namespace ppp {

void Shooter::TibuFoot() {
  if (idx == 1) {
    b.CommandBoth([](Servo& s) { s.SetStop(); });
    b.cmd_.mode = M::Idle_Nop;
    b.mags_.SetStrength(2, MagStren::Min);
    b.mags_.SetStrength(3, MagStren::Min);
  } else if (idx == 2) {
    b.CommandBoth([](Servo& s) { s.SetStop(); });
    b.cmd_.mode = M::Idle_Nop;
    b.mags_.SetStrength(2, MagStren::Max);
    b.mags_.SetStrength(3, MagStren::Max);
  } else if (idx == 3) {
    b.CommandBoth([](Servo& s) { s.SetStop(); });
    b.cmd_.mode = M::Idle_Nop;
    b.mags_.SetStrength(0, MagStren::Min);
    b.mags_.SetStrength(1, MagStren::Min);
  } else if (idx == 4) {
    b.CommandBoth([](Servo& s) { s.SetStop(); });
    b.cmd_.mode = M::Idle_Nop;
    b.mags_.SetStrength(0, MagStren::Max);
    b.mags_.SetStrength(1, MagStren::Max);
  }
}

void Shooter::RandomTibutibu() {
  if (idx == 23) {
    b.cmd_.mode = M::RandomMags_Init;
    b.cmd_.random_mags.min_phase_dur = 1000;
    b.cmd_.random_mags.max_phase_dur = 3000;
    b.cmd_.random_mags.dur = 10000;
  } else if (idx == 24) {
    b.cmd_.mode = M::RandomMags_Init;
    b.cmd_.random_mags.min_phase_dur = 10;
    b.cmd_.random_mags.max_phase_dur = 100;
    b.cmd_.random_mags.dur = 10000;
  }
}

}  // namespace ppp
