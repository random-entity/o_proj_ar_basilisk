#pragma once

#include "_meta.h"

void PPPShooter::SetGlobalVarSpeed(int level) {
  g::vars::speed = level == 0   ? g::c::speed::slower
                   : level == 1 ? g::c::speed::slow
                   : level == 2 ? g::c::speed::normal
                   : level == 3 ? g::c::speed::fast
                   : level == 4 ? g::c::speed::faster
                                : g::c::speed::fastest;
}

void PPPShooter::SetGlobalVarSpeedFiner(int level) {
  level = constrain(level, 0, 19);
  g::vars::speed = map(static_cast<double>(level),  //
                       0.0, 19.0,                   //
                       g::c::speed::sloth, g::c::speed::fastest);
  /*
    0:  0.025
    1:  0.050
    2:  0.075
    3:  0.100
    4:  0.125
    5:  0.150
    6:  0.175
    7:  0.200
    8:  0.225
    9:  0.250
    10: 0.275
    11: 0.300
    12: 0.325
    13: 0.350
    14: 0.375
    15: 0.400
    16: 0.425
    17: 0.450
    18: 0.475
    19: 0.500
  */
}
