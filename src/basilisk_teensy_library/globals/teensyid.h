#pragma once

#include <Arduino.h>

#include <map>

#include "debug.h"
#include "serials.h"

#define TEENSYID_REGISTER_0 (0x401F4410)
#define TEENSYID_REGISTER_1 (0x401F4420)

uint64_t GetTeensyId() {
  union {
    uint64_t matome;
    uint32_t chunk[2];
  } teensyid;

  teensyid.chunk[0] = *(volatile uint32_t*)TEENSYID_REGISTER_0;
  teensyid.chunk[1] = *(volatile uint32_t*)TEENSYID_REGISTER_1;

#if DEBUG_SETUP
  P("TeensyID -> 0x");
  Serial.printf("%016llX\n", teensyid.matome);
#endif

  return teensyid.matome;
}

namespace g::teensyid {
const std::map<uint64_t, int> to_suid = {
    {0x322F51D7653F900A, 1},   //
    {0x3C18A1D265F8296A, 2},   //
    {0x134231D2677F0531, 3},   //
    {0x162111D764FE06E6, 4},   //
    {0x271311D764FE06E6, 5},   //
    {0x2D4731D2677F0531, 6},   //
    {0x212511D764FE06E6, 7},   //
    {0x204381D2677F054C, 8},   //
    {0x353B81D2677F054C, 9},   //
    {0x2A4231D2677F0531, 10},  //
    {0x313511D764FE06E6, 11},  //
    {0x422511D764FE06E6, 12},  //
    {0x3B2A51D7653F900A, 13},  //
    /////////////////////////////
    {0x232411D764FE06E6, KAKTUGI_14_SUID},  // Kaktugi
};
}  // namespace g::teensyid
