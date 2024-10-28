#pragma once

#include <Arduino.h>

#include "basilisk.h"
#include "cmd_rcvrs/neokey_cr.h"
#include "cmd_rcvrs/xbee_cr.h"
#include "components/neokey.h"
#include "components/specifics/neokey1x4_i2c0.h"
#include "executer.h"
#include "globals/serials.h"
#include "rpl_sndrs/led_rs.h"
#include "rpl_sndrs/serial_rs.h"
#include "rpl_sndrs/xbee_rs.h"

namespace parasite {

void main(const Basilisk::Configuration& cfg) {
  // The Basilisk instance.
  Basilisk b{cfg};

  // Neokey.
  Neokey& nk = specifics::neokey1x4_i2c0;

  // CommandReceivers.
  NeokeyCommandReceiver nkcr{nk, b};
  XbeeCommandReceiver xbcr{b};

  // ReplySenders.
#if DEBUG_SERIAL_RS
  SerialReplySender serrs{b};
#endif
  LedReplySender ledrs{b, nk};
  XbeeReplySender xbrs{b};

  // The Executer.
  Executer exec{b, nkcr, xbcr};

#if DEBUG_XBEE_TIMING
  P("XbRS timing -> ");
  for (int i = 0; i < 13; i++) {
    Serial.print(g::xb::Timing::mod13_to_send_time_us.at(i));
    P(", ");
  }
  P("span -> ");
  Serial.println(g::xb::Timing::span);
#endif

#if DEBUG_SETUP
  Pln("setup() done!");
  Pln("=^-^=^-^=^-^=");
#endif

  while (1) {
    nkcr.Run();
    xbcr.Run();
    b.Run();
    exec.Run();
#if DEBUG_SERIAL_RS
    serrs.Run();
#endif
    ledrs.Run();
    xbrs.Run();
  }
}

}  // namespace parasite
