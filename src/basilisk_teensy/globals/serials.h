#pragma once

#include <Arduino.h>

#include "../helpers/do_you_want_debug.h"
#include "../helpers/serial_print.h"

#define SERIAL_BEGIN_WAIT_TIME_MS (250)

#define ENABLE_SERIAL                                            \
  (DEBUG_INITIALIZATION || DEBUG_TEENSYID || DEBUG_SERIAL_RS ||  \
   DEBUG_XBEE_TIMING || DEBUG_XBEE_RECEIVE || DEBUG_XBEE_SEND || \
   DEBUG_NEOKEYCR)

#define SERIAL_BAUDRATE (9600)

#define IMU_SERIAL (Serial2)
#define IMU_SERIAL_BAUDRATE (57600)

#define LPS_SERIAL (Serial6)
#define LPS_SERIAL_BAUDRATE (9600)

#define XBEE_SERIAL_BAUDRATE (115200)

#if ENABLE_SERIAL
// Just initialize once and ignore Serial.begin() failure.
void InitSerial() {
  static bool imiham = false;
  if (!imiham) {
    Serial.begin(SERIAL_BAUDRATE);
    delay(SERIAL_BEGIN_WAIT_TIME_MS);
    Pln("********************************************************");
    Pln("*******************GOOD MORNING FOLKS*******************");
    Pln("******************BASILISK HAS ARRIVED******************");
    imiham = true;
  }
}
#endif
