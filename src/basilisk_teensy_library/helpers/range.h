#pragma once

#include <Arduino.h>

template <typename T>
struct Range {
  Range(const T& _from, const T& _to) : from{_from}, to{_to} {}
  T from, to;
  bool operator==(const T& val) const { return from <= val && val <= to; }
  friend bool operator==(const T& val, const Range<T>& range) {
    return range == val;
  }
};
