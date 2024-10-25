#pragma once

#include "../basilisk.h"

// Useless except usage by NeokeyCommandReceiver.
void Shoot(Basilisk& b) {
  using O = Basilisk::Command::Oneshot;
  auto& o = b.cmd_.oneshot;

  switch (o) {
    case O::CRMuxXbee: {
      // Processed immediately at reception if sent as B-PPP to XbeeCR
      // without setting Basilisk::oneshot.
      b.crmux_ = Basilisk::CRMux::Xbee;
    } break;
    case O::SetBaseYaw: {
      // Processed immediately at reception if sent as B-PPP to XbeeCR
      // without setting Basilisk::oneshot.
      b.imu_.SetBaseYaw(b.cmd_.set_base_yaw.offset);
    } break;
    case O::Inspire: {
      // Not yet implemented.
    } break;
    case O::TimeSlottedPoll: {
      // Processed immediately at reception if sent as Oneshot to XbeeCR
      // without setting Basilisk::oneshot.
      b.poll_clk_us_ = 0;
    } break;
  }

  o = O::None;
}
