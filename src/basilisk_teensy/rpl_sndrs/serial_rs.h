#pragma once

#include "../roster/db.h"
#include "../servo_units/basilisk.h"

void SerialReplySender(Basilisk& b) {
  Serial.println("******************");
  Serial.println("SerialReplySender:");

  // Servo Replies
  b.CommandBoth([](Servo* s) { s->Print(); });

  // Mode
  Serial.print("Mode -> ");
  Serial.print(static_cast<uint8_t>(b.cmd_.mode));
  Serial.println();

  // Servo outputs
  Serial.print("phi_l:");
  Serial.print(b.l_.GetReply().abs_position, 4);
  Serial.print(",");
  Serial.print("phi_r:");
  Serial.print(b.r_.GetReply().abs_position, 4);
  Serial.println();

  // LPS position
  Serial.print("lpsx=");
  Serial.print(b.lps_.x_);
  Serial.print(";");
  Serial.print("lpsy=");
  Serial.print(b.lps_.y_);
  Serial.println();

  // LPS debug
  Serial.print("b.lps_.dists_raw_[0]:");
  Serial.print(b.lps_.dists_raw_[0]);
  Serial.print(",");
  Serial.print("b.lps_.dists_raw_[1]:");
  Serial.print(b.lps_.dists_raw_[1]);
  Serial.print(",");
  Serial.print("b.lps_.dists_raw_[2]:");
  Serial.print(b.lps_.dists_raw_[2]);
  Serial.println();
  Serial.print("b.lps_.error_.bytes[0]:");
  Serial.print(b.lps_.error_.bytes[0]);
  Serial.print(",");
  Serial.print("b.lps_.error_.bytes[1]:");
  Serial.print(b.lps_.error_.bytes[1]);
  Serial.print(",");
  Serial.print("b.lps_.error_.bytes[2]:");
  Serial.print(b.lps_.error_.bytes[2]);
  Serial.println();

  // IMU orientation
  // Serial.print("roll:");
  // Serial.print(b.imu_.euler_[0], 3);
  // Serial.print(",");
  // Serial.print("pitch:");
  // Serial.print(b.imu_.euler_[1], 3);
  // Serial.print(",");
  Serial.print("yaw:");
  Serial.print(b.imu_.GetYaw(true), 4);
  Serial.println();

  // Lego
  // Serial.print("contact_l=");
  // Serial.print(b.lego_.state_[0].contact, BIN);
  // Serial.print("/");
  // Serial.print("contact_r=");
  // Serial.print(b.lego_.state_[1].contact, BIN);
  // Serial.println();

  // Magnets
  // Serial.print("time_since_last_attach[0]=");
  // Serial.print(b.mags_.time_since_last_attach_[0]);
  // Serial.print("/");
  // Serial.print("time_since_last_attach[1]=");
  // Serial.print(b.mags_.time_since_last_attach_[1]);
  // Serial.print("/");
  // Serial.print("time_since_last_attach[2]=");
  // Serial.print(b.mags_.time_since_last_attach_[2]);
  // Serial.print("/");
  // Serial.print("time_since_last_attach[3]=");
  // Serial.print(b.mags_.time_since_last_attach_[3]);
  // Serial.println();

  // Roster
  Serial.println("Roster");
  for (uint8_t suid = 1; suid <= 13; suid++) {
    Serial.print(suid);
    Serial.print(" -> ");
    if (suid == b.cfg_.suid) {
      Serial.print("self");
    } else {
      Serial.print(roster::db[suid - 1].x);
      Serial.print(", ");
      Serial.print(roster::db[suid - 1].y);
      Serial.print(", ");
      Serial.print(roster::db[suid - 1].yaw);
      Serial.print(", ");
      Serial.print((micros() - roster::updated_time[suid - 1]) * 1e-6, 6);
    }
    Serial.print(";  ");
    if (suid == 7) Serial.println();
  }
  Serial.println();
}
