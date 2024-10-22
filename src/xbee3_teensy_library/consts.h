#pragma once

#include <Arduino.h>

#define XB_START (0x7E)
#define XB_ESC (0x7D)
#define XB_XON (0x11)
#define XB_XOFF (0x13)

#define XB_FRAME_TYPE_TXREQ (0x10)

#define MAX_FRAME_DATA_SIZE (110)

#define BROADCAST_ADDRESS (0xFFFF)
