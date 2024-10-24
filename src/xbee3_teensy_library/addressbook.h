#pragma once

#include <Arduino.h>

#include <map>

namespace xb {

const std::map<uint64_t, int> addr_to_suid{
    {0x0013A20041C22F0E, 1},   //
    {0x0013A20041C22D2A, 2},   //
    {0x0013A20041C2AF68, 4},   //
    {0x0013A20041C15729, 5},   //
    {0x0013A20041C22F71, 9},   //
    {0x0013A20041BEA1C1, 10},  //
    {0x0013A20041C2F781, 13},  //

    {0x0013A20041C2E64D, 50},  // Commander
    {0x0013A20041C14EC5, 51},  // Monitor
};

}  // namespace xb
