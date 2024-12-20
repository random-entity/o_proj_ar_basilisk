#pragma once

#include "../basilisk.h"
#include "../ppp/_matome.h"

struct OneshotShooters {
  using O = Basilisk::Command::Oneshots::ByteRep;

  OneshotShooters(Basilisk& _b) : b{_b} {}

  void Shoot() {
    auto& o = b.cmd_.oneshots;

    if (o.Has(O::SetBaseYaw)) {
      b.imu_.SetBaseYaw(b.cmd_.set_base_yaw.offset);
      o.Remove(O::SetBaseYaw);
    }

    if (o.Has(O::BPPP)) {
      pppshooter.Shoot();
      o.Remove(O::BPPP);
    }

    if (o.Has(O::SaveRealBaseYaw)) {
      b.imu_.SaveRealBaseYaw();
      o.Remove(O::SaveRealBaseYaw);
    }

    if (o.Has(O::RestoreRealBaseYaw)) {
      b.imu_.RestoreRealBaseYaw();
      o.Remove(O::RestoreRealBaseYaw);
    }
  }

  Basilisk& b;
  PPPShooter pppshooter{b};
};
