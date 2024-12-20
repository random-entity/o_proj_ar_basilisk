#pragma once

#include <Arduino.h>

#include "../helpers/clamped.h"

namespace g::c {

namespace speed {
const PhiSpeed sloth = 0.025;
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
const PhiAccLim standard = 2.0;
const PhiAccLim stiff = 4.0;
}  // namespace acclim

namespace maxdur {
constexpr uint32_t safe = 3000;
constexpr uint32_t yoyuu = 5000;
constexpr uint32_t beche = 8000;
}  // namespace maxdur

namespace phithr {
namespace fix {
constexpr double standard = 0.005;
}  // namespace fix
namespace damp {
constexpr double standard = 0.05;
}  // namespace damp
}  // namespace phithr

}  // namespace g::c
