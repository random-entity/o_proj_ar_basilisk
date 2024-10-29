#pragma once

#include "../basilisk.h"
#include "../ppp/_meta.h"

struct OneshotShooters {
  using O = Basilisk::Command::Oneshot::ByteRep;

  OneshotShooters(Basilisk& _b) : b{_b} {}

  void Shoot() {
    auto& o = b.cmd_.oneshots;

    if (o.Has(O::SetBaseYaw)) {
      b.imu_.SetBaseYaw(b.cmd_.set_base_yaw.offset);
      o.Remove(O::SetBaseYaw);
    }

    if (o.Has(O::BPPP)) {
      ppp.byterep_to_function.at(O::BPPP)();
      o.Remove(O::BPPP);
    }
  }

  Basilisk& b;
  PPP ppp{b};
};
