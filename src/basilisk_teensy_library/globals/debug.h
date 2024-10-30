#pragma once

#include <Arduino.h>

#define DEBUG_SETUP (1)
#define DEBUG_FAILURE (1)
#define DEBUG_SERVOS (1)
#define DEBUG_XBEE (1)
#define DEBUG_NEOKEYCR (0)
#define DEBUG_SERIALRS (0)
// Add to the OR chain of ENABLE_SERIAL/ whenever adding a new flag.

#define ENABLE_SERIAL                                            \
  (DEBUG_SETUP || DEBUG_FAILURE || DEBUG_SERVOS || DEBUG_XBEE || \
   DEBUG_NEOKEYCR || DEBUG_SERIALRS)

#if ENABLE_SERIAL
void P(const char* str) { Serial.print(F(str)); }
void Pln(const char* str) { Serial.println(F(str)); }
#endif

void HALT(const char* err_str = nullptr) {
#if ENABLE_SERIAL
  Pln("x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x");
  if (err_str) Pln(err_str);
  Pln("Halting program");
#endif

  while (1);
}

#define KAKTUGI_14_SUID (12)

#define MOCK_AUX2 (0)
#define MOCK_LPS (0)
#define MOCK_IMU (0)
