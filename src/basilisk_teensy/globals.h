#pragma once

#include <Arduino.h>

#include "helpers/clamped.h"
#include "helpers/using_moteus.h"

namespace globals {

elapsedMicros poll_clk_us;

namespace var {
PhiSpeed speed = 0.1;
}

namespace stdval {

namespace speed {
const PhiSpeed slower = 0.05;
const PhiSpeed slow = 0.075;
const PhiSpeed normal = 0.1;
const PhiSpeed fast = 0.2;
const PhiSpeed faster = 0.3;
const PhiSpeed fastest = 0.5;
const PhiSpeed impossible = 0.75;
const PhiSpeed yunakim = 1.0;
}  // namespace speed

namespace acclim {
PhiAccLim standard = 2.0;
PhiAccLim stiff = 4.0;
}  // namespace acclim

namespace maxdur {
uint32_t safe = 5000;
}

}  // namespace stdval

}  // namespace globals
