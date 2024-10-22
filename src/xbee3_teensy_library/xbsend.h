#pragma once

#include <Arduino.h>

#include "consts.h"

/* Assumes API with escape as radio operating mode. */
class XbeeSender {
 public:
  XbeeSender() { frame[0] = XB_START; }

  void Send(const uint8_t* frame_data, const int& len) {
    int idx = 1;
    frame[idx++] = len & 0xFF00;
    frame[idx++] = len & 0xFF;
  }

  uint8_t frame[MAX_FRAME_DATA_SIZE];
};

// void Send(const uint64_t& dest_addr,  //
//           const uint8_t* const data, const uint32_t& len) {
//   static uint8_t frame[XB_MAX_PACKET_LEN] = {XB_STDLM};
//   int idx = 1;
// }
