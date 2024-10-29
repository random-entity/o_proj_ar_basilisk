#pragma once

#include <Arduino.h>

#define DEBUG_SETUP (1)
#define DEBUG_TEENSYID (1)
#define DEBUG_SUID (1)
#define DEBUG_SERVOS (1)
#define DEBUG_FAILURE (1)
#define DEBUG_SERIAL_RS (1)
#define DEBUG_XBEE_TIMING (1)
#define DEBUG_XBEE_RECEIVE (1)
#define DEBUG_XBEE_SEND (1)
#define DEBUG_NEOKEYCR (1)
// Add to the OR chain of ENABLE_SERIAL in globals/serials.h
// whenever adding a new flag.

#define TEST_NO_AUX2 (0)
