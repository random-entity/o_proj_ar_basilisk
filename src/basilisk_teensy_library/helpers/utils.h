#pragma once

#include <Arduino.h>

#include <map>

#ifndef NaN
#define NaN (0.0 / 0.0)
#endif

constexpr uint64_t one_uint64 = static_cast<uint64_t>(1);

void incu8(uint8_t& n) {
  if (n != 0xFF) n++;
}

void incu32(uint32_t& n) {
  if (n != 0xFFFFFFFF) n++;
}

int PosMod(const int& a, const int& b) {
  int r = a % b;
  while (r < 0) {
    r += abs(b);
  }
  return r;
}

double signedpow(const double& base, const float& exponent) {
  const auto y = pow(abs(base), exponent);
  return base >= 0 ? y : -y;
}

double nearest_pmn(const double& tgt, double var) {
  if (isnan(tgt) || isnan(var)) return NaN;
  if (var == tgt) return var;
  if (var > tgt) {
    while (var > tgt + 0.5) var -= 1.0;
  } else {
    while (var < tgt - 0.5) var += 1.0;
  }
  return var;
}

template <typename K, typename V>
V* SafeAt(const std::map<K, V*>& map, const K& key) {
  auto it = map.find(key);
  if (it == map.end()) return nullptr;
  return it->second;
}
