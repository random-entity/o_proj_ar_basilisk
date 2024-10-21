#pragma once

#include <Arduino.h>

#ifndef XBEE_SERIAL
#define XBEE_SERIAL (Serial4)
#endif

#define XB_MAX_PACKET_LEN (50)
#define XB_STDLM (0x7E)
#define XB_ESC (0x7D)
#define XB_XON (0x11)
#define XB_XOFF (0x13)
#define XB_FRAMETYPE_TX_REQ (0x10)

/* Assumes API with escape as radio operating mode. */
namespace xb {

void Send(const uint64_t& dest_addr,  //
          const uint8_t* const data, const uint32_t& len) {
  static uint8_t frame[XB_MAX_PACKET_LEN] = {XB_STDLM};
  int idx = 1;
}

void Receive() {}

}  // namespace xb
