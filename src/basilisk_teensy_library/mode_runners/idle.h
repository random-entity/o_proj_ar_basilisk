#pragma once

#include "_meta.h"

void ModeRunners::Idle() {
  auto& m = b.cmd_.mode;

  switch (m) {
    case M::Idle_Init: {
      b.CommandBoth([](Servo& s) { s.SetStop(); });
      b.mags_.AttachAll();
      m = M::Idle_Nop;
    } break;
    case M::Idle_Nop: {
      // Rest in peace.
    } break;
    default:
      break;
  }
}
