#include <Arduino.h>

#include "basilisk.h"
// #include "cmd_rcvrs/neokey_cr.h"
#include "components/neokey.h"
#include "components/specifics/neokey1x4_i2c0.h"
#include "globals/serials.h"
// #include "globals/timing.h"
// #include "helpers/beat.h"
#include "executer.h"
#include "helpers/serial_print.h"
#include "rpl_sndrs/serial_rs.h"

// Basilisk configuration.
Basilisk::Configuration cfg{
    .lps{.c = 300.0,
         .x_c = 150.0,
         .y_c = 300.0,
         .minx = 50.0,
         .maxx = 250.0,
         .miny = 50.0,
         .maxy = 250.0},
    .lego{.run_interval = 20},
    .mags{.run_interval = 100},
    .collision_thr{100.0},
    .overlap_thr{50.0},
};

Basilisk b{cfg};
Executer exec{&b};
Neokey& nk = specifics::neokey1x4_i2c0;
// NeokeyCommandReceiver nkcr{nk, b};

void setup() {
#if ENABLE_SERIAL
  InitSerial();
#endif

  // #if DEBUG_XBEE_TIMING
  //   P("XbRS timing -> ");
  //   for (uint8_t suid = 1; suid <= 13; suid++) {
  //     Serial.print(timing::xb::send_times_us.at(suid));
  //     P(", ");
  //   }
  //   P("span -> ");
  //   Serial.println(timing::xb::span);
  // #endif

  CanFdDriverInitializer::Setup(1);
  delay(100);
  Servo l{1, 1, &moteus_fmt::pm_fmt, &moteus_fmt::q_fmt};
  delay(100);
  l.SetStop();
  delay(100);
  l.SetQuery();
  delay(100);
  l.Print();

  //   if (!b.Setup()) {
  //     nk.setPixelColor(0, 0xF00000);
  // #if DEBUG_SETUP
  //     Pln("Basilisk initialization failed");
  //     Pln("*x*x*x*x*x*x*x*x*x*x*x*x*x*x*x");
  // #endif
  //     while (1);
  //   }

  // nkcr.Setup();

#if DEBUG_SETUP
  Pln("setup() done!");
  Pln("=^-^=^-^=^-^=");
#endif
}

void loop() {
  // b.Run();

  // static Beat exec_beat{10};
  // if (exec_beat.Hit()) exec.Run();

  // static Beat nkcr_beat{NeokeyCommandReceiver::run_interval_ms_};
  // if (nkcr_beat.Hit()) nkcr.Run();

  // #if DEBUG_SERIAL_RS
  // static Beat serial_rs_beat{1000};
  // if (serial_rs_beat.Hit()) SerialReplySender(b);
  // #endif
}
