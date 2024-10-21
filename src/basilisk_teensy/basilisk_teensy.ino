#include "cmd_rcvrs/neokey_cr.h"
#include "components/neokey.h"
#include "components/specifics/neokey1x4_i2c0.h"

// Basilisk configuration.
Basilisk::Configuration cfg{
    .suid =
        [] {
          uint8_t suid = 0;
          // const auto teensyid = GetTeensyId();
          // if (teensyid_to_suid.find(teensyid) != teensyid_to_suid.end()) {
          //   suid = teensyid_to_suid.at(teensyid);
          // }
          return suid;
        }(),  //
    .servo{.id_l = 1, .id_r = 2, .bus = 1},
    .lps{.c = 860.0,
         .x_c = 430.0,
         .y_c = 910.0,
         .minx = 100.0,
         .maxx = 760.0,
         .miny = 100.0,
         .maxy = 810.0},
    .lego{.pin_l = 23, .pin_r = 29, .run_interval = 20},  //
    .mags{.pin_la = 3,
          .pin_lt = 4,
          .pin_ra = 5,
          .pin_rt = 6,
          .run_interval = 100}};

Basilisk b{cfg};
Neokey& nk = specifics::neokey1x4_i2c0;
NeokeyCommandReceiver nkcr{nk, b};

void setup() {
  Serial.begin(9600);
  delay(250);

  nkcr.Setup();
}

void loop() {
  nkcr.Run();
  nkcr.Parse();
  nkcr.nk_cmd_ = 0;
}
