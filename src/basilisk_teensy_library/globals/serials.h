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

namespace g::serials {

inline constexpr uint32_t common_begin_wait_time = 250;

struct HwSerial {
  HwSerial(HardwareSerialIMXRT& _s, const uint32_t& _baudrate)
      : s{_s}, baudrate{_baudrate} {
    s.begin(baudrate);
    delay(common_begin_wait_time);
  }
  HardwareSerialIMXRT& s;
  uint32_t baudrate;
};

HwSerial imu{Serial2, 57600};
HwSerial lps{Serial6, 9600};
HwSerial xb{Serial4, 115200};

}  // namespace g::serials

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
#endif
