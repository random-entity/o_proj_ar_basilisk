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

namespace parasite {

void main() {}

// Basilisk configuration overrides.
Basilisk::Configuration cfg{
    .lps{.c = 300.0,
         .x_c = 150.0,
         .y_c = 300.0,
         .minx = 50.0,
         .maxx = 250.0,
         .miny = 50.0,
         .maxy = 250.0},
};

// The Basilisk instance.
Basilisk b{cfg};

// CommandReceivers.
Neokey& nk = specifics::neokey1x4_i2c0;
NeokeyCommandReceiver nkcr{nk, b};
XbeeCommandReceiver xbcr{b};

// ReplySenders.
#if DEBUG_SERIAL_RS
SerialReplySender serrs{b};
#endif
LedReplySender ledrs{b, nk};

// The Executer.
Executer exec{b, nkcr, xbcr};

void setup() {
  // #if DEBUG_XBEE_TIMING
  //   P("XbRS timing -> ");
  //   for (uint8_t suid = 1; suid <= 13; suid++) {
  //     Serial.print(timing::xb::mod13_to_send_time_us.at(suid));
  //     P(", ");
  //   }
  //   P("span -> ");
  //   Serial.println(timing::xb::span);
  // #endif

  if (!nkcr.Setup()) {
#if DEBUG_SETUP
    Pln("Neokey initialization failed");
    Pln("x_x_x_x_x_x_x_x_x_x_x_x_x_x_");
#endif
    while (1);
  }

  if (!b.Setup()) {
    for (int i = 0; i < 4; i++) nk.setPixelColor(i, 0xF00000);
    nk.show();
#if DEBUG_SETUP
    Pln("Basilisk initialization failed");
    Pln("x_x_x_x_x_x_x_x_x_x_x_x_x_x_x_");
#endif
    while (1);
  }

#if DEBUG_SETUP
  Pln("setup() done!");
  Pln("=^-^=^-^=^-^=");
#endif
}

void loop() {
  nkcr.Run();
  xbcr.Run();
  b.Run();
  exec.Run();
  ledrs.Run();
#if DEBUG_SERIAL_RS
  serrs.Run();
#endif
}

}  // namespace parasite
