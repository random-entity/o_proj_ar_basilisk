#pragma once

#include <Arduino.h>

#include "debug.h"

namespace g::serials {

inline constexpr uint32_t common_begin_wait_time = 200;

struct HardwareSerialWrapper {
  HardwareSerialWrapper(HardwareSerial& _s, const uint32_t& _baudrate)
      : s{_s}, baudrate{_baudrate} {
    s.begin(baudrate);
    delay(common_begin_wait_time);
  }

  operator HardwareSerial&() { return s; }
  operator bool() { return s; }

  HardwareSerial& s;
  const uint32_t baudrate;
};

/* IMU, LPS, XBee Serials are all initialized here already. */
HardwareSerialWrapper imu{Serial2, 57600};
HardwareSerialWrapper lps{Serial6, 9600};
HardwareSerialWrapper xb{Serial4, 115200};

}  // namespace g::serials

#define ENABLE_SERIAL                                                \
  (DEBUG_SETUP || DEBUG_TEENSYID || DEBUG_SERVOS || DEBUG_FAILURE || \
   DEBUG_SERIAL_RS || DEBUG_XBEE_TIMING || DEBUG_XBEE_RECEIVE ||     \
   DEBUG_XBEE_SEND || DEBUG_NEOKEYCR)

#if ENABLE_SERIAL
#define P(str) (Serial.print(F(str)))
#define Pln(str) (Serial.println(F(str)))

// Just initialize once and ignore Serial.begin() failure.
void InitSerial() {
  static bool imiham = false;
  if (!imiham) {
    Serial.begin(9600);
    delay(g::serials::common_begin_wait_time);
    Pln("|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_");
    Pln("*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.");
    Pln(".*.*.*.*.*.*.*.*.*.*.*.*GOOD MORNING FOLKS.*.*.*.*.*.*.*.*.*.*.*.*.*");
    Pln("*.*.*.*.*.*.*.*.*.*.*.*BASILISK HAS ARRIVED.*.*.*.*.*.*.*.*.*.*.*.*.");
    Pln(".*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*");
    Pln("^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~");
    imiham = true;
  }
}

namespace g::serials {
const bool serial_began = [] {
  /* Serial is initialized here already. */
  InitSerial();
  return Serial;
}();
}  // namespace g::serials
#endif
