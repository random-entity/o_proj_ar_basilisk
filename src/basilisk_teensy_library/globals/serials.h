#pragma once

#include <Arduino.h>

#include "debug.h"

namespace g::serials {
inline constexpr uint32_t common_begin_wait_time = 200;
}

#if ENABLE_SERIAL
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

namespace g::serials {

struct HardwareSerialWrapper {
  HardwareSerialWrapper(HardwareSerial& _ser, const uint32_t& _baudrate,
                        const char* name)
      : ser{_ser}, baudrate{_baudrate} {
    ser.begin(baudrate);
    delay(common_begin_wait_time);

#if DEBUG_SETUP
    P(name);
    Pln(": HardwareSerial began"
        "(operator bool() always returns true for HardwareSerialIMXRT's)");
#endif
  }

  operator HardwareSerial&() { return ser; }

  HardwareSerial& ser;
  const uint32_t baudrate;
};

/* IMU, LPS, XBee Serials are all initialized here already. */
HardwareSerialWrapper imu{Serial2, 57600, "IMU"};
HardwareSerialWrapper lps{Serial6, 9600, "LPS"};
HardwareSerialWrapper xb{Serial4, 115200, "Xbee"};

}  // namespace g::serials
