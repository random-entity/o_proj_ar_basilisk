#pragma once

#include "../basilisk.h"
#include "../helpers/beat.h"
#include "../roster.h"

class SerialReplySender {
 public:
  SerialReplySender(Basilisk& b, const uint32_t& run_interval = 1000)
      : b_{b}, r_{b.rpl_}, beat_{run_interval} {}

  void Run() {
    if (!beat_.Hit()) return;

    Pln("*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*");
    Pln("o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o");
    Pln("SerialReplySender:");

    Pln("***** Servos *****");
    b_.CommandBoth([](Servo& s) { s.Print(); });

    Pln("***** Mode *****");
    Serial.println(r_.mode());

    Pln("***** Phis *****");
    P("phi_l:");
    Serial.print(r_.phi_l(), 4);
    P(",");
    P("phir:");
    Serial.print(r_.phi_r(), 4);
    Serial.println();

    Pln("***** LPS *****");
    P("lpsx:");
    Serial.print(r_.lpsx());
    P(",");
    P("lpsy:");
    Serial.print(r_.lpsy());
    Serial.println();

    P("raw -> ");
    Serial.print(b_.lps_.dists_raw_[0]);
    P(" / ");
    Serial.print(b_.lps_.dists_raw_[1]);
    P(" / ");
    Serial.print(b_.lps_.dists_raw_[2]);
    Serial.println();

    P("err -> ");
    Serial.print(b_.lps_.error_.bytes[0]);
    P(" / ");
    Serial.print(b_.lps_.error_.bytes[1]);
    P(" / ");
    Serial.print(b_.lps_.error_.bytes[2]);
    Serial.println();

    P("since raw update -> ");
    Serial.print(r_.lps_since_raw_update() * 1e-3, 3);
    Serial.println();

    Pln("***** IMU *****");
    P("roll:");
    Serial.print(b_.imu_.euler_[0], 3);
    P(",");
    P("pitch:");
    Serial.print(b_.imu_.euler_[1], 3);
    P(",");
    P("yaw:");
    Serial.print(r_.yaw(), 4);
    Serial.println();

    P("since update -> ");
    Serial.print(r_.imu_since_update() * 1e-3, 3);
    Serial.println();

    Pln("***** Lego *****");
    P("contact -> ");
    Serial.print(b_.lego_.state_[0].contact, BIN);
    P(" / ");
    Serial.print(b_.lego_.state_[1].contact, BIN);
    Serial.println();

    Pln("***** Magnets *****");
    P("since_attach -> ");
    Serial.print(b_.mags_.since_attach_[0]);
    P(" / ");
    Serial.print(b_.mags_.since_attach_[1]);
    P(" / ");
    Serial.print(b_.mags_.since_attach_[2]);
    P(" / ");
    Serial.print(b_.mags_.since_attach_[3]);
    Serial.println();

    Pln("***** Roster *****");
    for (int suid = 1; suid <= 13; suid++) {
      Serial.print(suid);
      P(" -> ");
      if (suid == r_.suid) {
        P("self");
      } else {
        Serial.print(roster[suid - 1].x);
        P(" / ");
        Serial.print(roster[suid - 1].y);
        P(" / ");
        Serial.print(roster[suid - 1].yaw);
        P(" / ");
        Serial.print(roster[suid - 1].since_update_us * 1e-6, 6);
      }
      P(" | ");
      if (suid == 7) Serial.println();
    }
    Serial.println();
  }

 private:
  Basilisk& b_;
  Basilisk::Reply& r_;
  Beat beat_;
};
