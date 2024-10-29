#pragma once

#include <Arduino.h>

#include "debug.h"

namespace g::serials {
inline constexpr uint32_t common_begin_wait_time = 200;
}

#define ENABLE_SERIAL                                             \
  (DEBUG_SETUP || DEBUG_TEENSYID || DEBUG_SUID || DEBUG_SERVOS || \
   DEBUG_FAILURE || DEBUG_SERIAL_RS || DEBUG_XBEE_TIMING ||       \
   DEBUG_XBEE_RECEIVE || DEBUG_XBEE_SEND || DEBUG_NEOKEYCR)

#if ENABLE_SERIAL
void P(const char* str) { Serial.print(F(str)); }
void Pln(const char* str) { Serial.println(F(str)); }

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

void HALT(const char* err_str = nullptr) {
#if ENABLE_SERIAL
  Pln("x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x");
  if (err_str) Pln(err_str);
  Pln("Halting program");
#endif

  while (1);
}

namespace g::serials {

struct HardwareSerialWrapper {
  HardwareSerialWrapper(HardwareSerial& _ser, const uint32_t& _baudrate,
                        const char* name)
      : ser{_ser}, baudrate{_baudrate} {
#if DEBUG_SETUP
    P(name);
    Pln(": Beginning HardwareSerial");
#endif

    ser.begin(baudrate);
    delay(common_begin_wait_time);

    if (!ser) {
      P(name);
      HALT(": Serial begin failed");
    }

#if DEBUG_SETUP
    P(name);
    Pln(": HardwareSerial began");
#endif
  }

  operator HardwareSerial&() { return ser; }
  operator bool() { return ser; }

  HardwareSerial& ser;
  const uint32_t baudrate;
};

/* IMU, LPS, XBee Serials are all initialized here already. */
HardwareSerialWrapper imu{Serial2, 57600, "IMU"};
HardwareSerialWrapper lps{Serial6, 9600, "LPS"};
HardwareSerialWrapper xb{Serial4, 115200, "Xbee"};

}  // namespace g::serials
