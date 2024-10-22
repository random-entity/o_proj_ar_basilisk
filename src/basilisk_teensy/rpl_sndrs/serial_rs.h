#pragma once

#include "../basilisk.h"
#include "../roster.h"

void SerialReplySender(Basilisk& b) {
  Pln("*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*");
  Pln("o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o*o");
  Pln("SerialReplySender:");

  Pln("***** Servos *****");
  b.CommandBoth([](Servo* s) { s->Print(); });

  Pln("***** Mode *****");
  Serial.println(static_cast<uint8_t>(b.cmd_.mode));

  Pln("***** Phis *****");
  P("phil:");
  Serial.print(b.l_.GetReply().abs_position, 4);
  P(",");
  P("phir:");
  Serial.print(b.r_.GetReply().abs_position, 4);
  Serial.println();

  Pln("***** LPS *****");
  P("lpsx:");
  Serial.print(b.lps_.x_);
  P(",");
  P("lpsy:");
  Serial.print(b.lps_.y_);
  Serial.println();

  P("raw -> ");
  Serial.print(b.lps_.dists_raw_[0]);
  P(" / ");
  Serial.print(b.lps_.dists_raw_[1]);
  P(" / ");
  Serial.print(b.lps_.dists_raw_[2]);
  Serial.println();

  P("err -> ");
  Serial.print(b.lps_.error_.bytes[0]);
  P(" / ");
  Serial.print(b.lps_.error_.bytes[1]);
  P(" / ");
  Serial.print(b.lps_.error_.bytes[2]);
  Serial.println();

  Pln("***** IMU *****");
  P("roll:");
  Serial.print(b.imu_.euler_[0], 3);
  P(",");
  P("pitch:");
  Serial.print(b.imu_.euler_[1], 3);
  P(",");
  P("yaw:");
  Serial.print(b.imu_.GetYaw(true), 4);
  Serial.println();

  Pln("***** Lego *****");
  P("contact -> ");
  Serial.print(b.lego_.state_[0].contact, BIN);
  P(" / ");
  Serial.print(b.lego_.state_[1].contact, BIN);
  Serial.println();

  Pln("***** Magnets *****");
  P("since_attach -> ");
  Serial.print(b.mags_.since_attach_[0]);
  P(" / ");
  Serial.print(b.mags_.since_attach_[1]);
  P(" / ");
  Serial.print(b.mags_.since_attach_[2]);
  P(" / ");
  Serial.print(b.mags_.since_attach_[3]);
  Serial.println();

  Pln("***** Roster *****");
  for (uint8_t suid = 1; suid <= 13; suid++) {
    Serial.print(suid);
    P(" -> ");
    if (suid == b.cfg_.suid) {
      P("self");
    } else {
      Serial.print(roster[suid - 1].x);
      P(" / ");
      Serial.print(roster[suid - 1].y);
      P(" / ");
      Serial.print(roster[suid - 1].yaw);
      P(" / ");
      Serial.print(roster[suid - 1].time_since_update_us * 1e-6, 6);
    }
    P(" | ");
    if (suid == 7) Serial.println();
  }
  Serial.println();
}
