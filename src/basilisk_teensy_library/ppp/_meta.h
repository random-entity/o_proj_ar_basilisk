#pragma once

#include <Arduino.h>

#include <functional>
#include <map>

#include "../basilisk.h"

struct PPP {
  using O = Basilisk::Command::Oneshots::ByteRep;

  PPP(Basilisk& _b) : b{_b} {}

  const std::map<O, std::function<void()>> byterep_to_function = {};

  Basilisk& b;
};
