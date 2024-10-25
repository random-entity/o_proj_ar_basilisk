#pragma once

#include <Arduino.h>

#include <unordered_set>

#include "../helpers/serial_print.h"

#define DEBUG_SETUP (1)
#define DEBUG_TEENSYID (1)
#define DEBUG_FAILURE (1)
#define DEBUG_SERIAL_RS (1)
#define DEBUG_XBEE_TIMING (1)
#define DEBUG_XBEE_RECEIVE (1)
#define DEBUG_XBEE_SEND (1)
#define DEBUG_NEOKEYCR (1)
// Add to the OR chain of ENABLE_SERIAL whenever adding a new flag.
#define ENABLE_SERIAL                                                   \
  (DEBUG_SETUP || DEBUG_TEENSYID || DEBUG_FAILURE || DEBUG_SERIAL_RS || \
   DEBUG_XBEE_TIMING || DEBUG_XBEE_RECEIVE || DEBUG_XBEE_SEND ||        \
   DEBUG_NEOKEYCR)

#define COMMON_SERIAL_BEGIN_WAIT_TIME (250)

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
    delay(COMMON_SERIAL_BEGIN_WAIT_TIME);
    Pln("|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_");
    Pln("*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.");
    Pln(".*.*.*.*.*.*.*.*.*.*.*.*GOOD MORNING FOLKS.*.*.*.*.*.*.*.*.*.*.*.*.*");
    Pln("*.*.*.*.*.*.*.*.*.*.*.*BASILISK HAS ARRIVED.*.*.*.*.*.*.*.*.*.*.*.*.");
    Pln(".*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*");
    Pln("^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~");
    imiham = true;
  }
}
#endif

void InitSerial(HardwareSerial* serial, const uint32_t& baudrate) {
  static std::unordered_set<HardwareSerial*> imiham;
  if (imiham.find(serial) != imiham.end()) return;
  serial->begin(baudrate);
  delay(COMMON_SERIAL_BEGIN_WAIT_TIME);
  imiham.insert(serial);
}
