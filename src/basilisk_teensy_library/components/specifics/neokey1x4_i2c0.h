#pragma once

#include "../neokey.h"

// Assuming specific setup:
// I2C0        col 0
// row y         x    0 1 2 3  addr
//   0 0  _neokeys[0]{0 1 2 3} 0x30

#ifndef I2C_BUS0
#define I2C_BUS0 (&Wire)
#endif

namespace specifics {

Adafruit_NeoKey_1x4 neokey1x4_i2c0_mtx[1][4 / 4] = {
    Adafruit_NeoKey_1x4{0x30, I2C_BUS0}};

Neokey neokey1x4_i2c0{(Adafruit_NeoKey_1x4*)neokey1x4_i2c0_mtx,  //
                      1, 4 / 4};

}  // namespace specifics
