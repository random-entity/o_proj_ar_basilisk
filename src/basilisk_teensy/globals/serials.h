#pragma once

#include <Arduino.h>

#include "../helpers/do_you_want_debug.h"
#include "../helpers/serial_print.h"

#define SERIAL_BAUDRATE (9600)

#define IMU_SERIAL (Serial2)
#define IMU_SERIAL_BAUDRATE (57600)

#define LPS_SERIAL (Serial6)
#define LPS_SERIAL_BAUDRATE (9600)

#define XBEE_SERIAL_BAUDRATE (115200)

#if ENABLE_SERIAL
// Just initialize once and ignore Serial.begin() failure.
void InitSerial() {
  static bool imi = false;
  if (!imi) {
    Serial.begin(SERIAL_BAUDRATE);
    delay(250);
    Pln("******************GOOD MORNING FOLKS******************");
    imi = true;
  }
}
#endif
