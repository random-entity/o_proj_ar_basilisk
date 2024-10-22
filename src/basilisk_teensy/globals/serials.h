#pragma once

#include <Arduino.h>

#include "../helpers/serial_print.h"

#define DEBUG_INITIALIZATION (1)
#define DEBUG_TEENSYID (1)
#define DEBUG_FAILURE (1)
#define DEBUG_SERIAL_RS (1)
#define DEBUG_XBEE_TIMING (1)
#define DEBUG_XBEE_RECEIVE (1)
#define DEBUG_XBEE_SEND (1)
#define DEBUG_NEOKEYCR (1)
// Add to the OR chain of ENABLE_SERIAL whenever adding a new flag.
#define ENABLE_SERIAL                                            \
  (DEBUG_INITIALIZATION || DEBUG_TEENSYID || DEBUG_FAILURE ||    \
   DEBUG_SERIAL_RS || DEBUG_XBEE_TIMING || DEBUG_XBEE_RECEIVE || \
   DEBUG_XBEE_SEND || DEBUG_NEOKEYCR)

#define SERIAL_BEGIN_WAIT_TIME_MS (250)

#define SERIAL_BAUDRATE (9600)

#define IMU_SERIAL (Serial2)
#define IMU_SERIAL_BAUDRATE (57600)

#define LPS_SERIAL (Serial6)
#define LPS_SERIAL_BAUDRATE (9600)

#define XBEE_SERIAL (Serial4)
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
