#pragma once

#include "../roster/db.h"
#include "../servo_units/basilisk.h"

void SerialReplySender(Basilisk& b) {
  Pln("******************");
  Pln("SerialReplySender:");

  // Servo Replies
  b.CommandBoth([](Servo* s) { s->Print(); });

  // Mode
  P("Mode -> ");
  Serial.print(static_cast<uint8_t>(b.cmd_.mode));
  Serial.println();

  // Servo outputs
  P("phi_l:");
  Serial.print(b.l_.GetReply().abs_position, 4);
  P(",");
  P("phi_r:");
  Serial.print(b.r_.GetReply().abs_position, 4);
  Serial.println();

  // LPS position
  P("lpsx=");
  Serial.print(b.lps_.x_);
  P(";");
  P("lpsy=");
  Serial.print(b.lps_.y_);
  Serial.println();

  // LPS debug
  P("b.lps_.dists_raw_[0]:");
  Serial.print(b.lps_.dists_raw_[0]);
  P(",");
  P("b.lps_.dists_raw_[1]:");
  Serial.print(b.lps_.dists_raw_[1]);
  P(",");
  P("b.lps_.dists_raw_[2]:");
  Serial.print(b.lps_.dists_raw_[2]);
  Serial.println();
  P("b.lps_.error_.bytes[0]:");
  Serial.print(b.lps_.error_.bytes[0]);
  P(",");
  P("b.lps_.error_.bytes[1]:");
  Serial.print(b.lps_.error_.bytes[1]);
  P(",");
  P("b.lps_.error_.bytes[2]:");
  Serial.print(b.lps_.error_.bytes[2]);
  Serial.println();

  // IMU orientation
  // P("roll:");
  // Serial.print(b.imu_.euler_[0], 3);
  // P(",");
  // P("pitch:");
  // Serial.print(b.imu_.euler_[1], 3);
  // P(",");
  P("yaw:");
  Serial.print(b.imu_.GetYaw(true), 4);
  Serial.println();

  // Lego
  // P("contact_l=");
  // Serial.print(b.lego_.state_[0].contact, BIN);
  // P("/");
  // P("contact_r=");
  // Serial.print(b.lego_.state_[1].contact, BIN);
  // Serial.println();

  // Magnets
  // P("time_since_last_attach[0]=");
  // Serial.print(b.mags_.time_since_last_attach_[0]);
  // P("/");
  // P("time_since_last_attach[1]=");
  // Serial.print(b.mags_.time_since_last_attach_[1]);
  // P("/");
  // P("time_since_last_attach[2]=");
  // Serial.print(b.mags_.time_since_last_attach_[2]);
  // P("/");
  // P("time_since_last_attach[3]=");
  // Serial.print(b.mags_.time_since_last_attach_[3]);
  // Serial.println();

  // Roster
  Pln("Roster");
  for (uint8_t suid = 1; suid <= 13; suid++) {
    Serial.print(suid);
    P(" -> ");
    if (suid == b.cfg_.suid) {
      P("self");
    } else {
      Serial.print(roster::db[suid - 1].x);
      P(", ");
      Serial.print(roster::db[suid - 1].y);
      P(", ");
      Serial.print(roster::db[suid - 1].yaw);
      P(", ");
      Serial.print((micros() - roster::updated_time[suid - 1]) * 1e-6, 6);
    }
    P(";  ");
    if (suid == 7) Serial.println();
  }
  Serial.println();
}
