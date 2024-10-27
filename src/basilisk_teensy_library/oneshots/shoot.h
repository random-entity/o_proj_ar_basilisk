#pragma once

#include "../basilisk.h"

void Shoot(Basilisk& b) {
  using O = Basilisk::Command::Oneshot;
  auto& o = b.cmd_.oneshot;

  if (o == O::None) return;

  switch (o) {
    case O::CRMuxXbee: {
      // Processed immediately at reception if sent as B-PPP to XbeeCR
      // without setting Basilisk::oneshot.
      b.crmux_ = Basilisk::CRMux::Xbee;
    } break;
    case O::SetBaseYaw: {
      b.imu_.SetBaseYaw(b.cmd_.set_base_yaw.offset);
    } break;
    case O::Inspire: {
      // Not yet implemented. Not sure if needed.
    } break;
    case O::BroadcastedPoll: {
      // Processed immediately at reception if sent as Oneshot to XbeeCR
      // without setting Basilisk::oneshot. Somosomo immediate processing at
      // reception should work better for network timing related work
      // unless synchronization with Executor is absolutely necessary.
      b.since_bpoll_us_ = 0;
    } break;
    default:
      break;
  }

  o = O::None;
}
