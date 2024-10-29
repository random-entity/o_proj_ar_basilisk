#pragma once

#include <Arduino.h>

template <typename T>
struct Range {
  Range(const T& _from, const T& _to) : from{_from}, to{_to} {}

  bool operator==(const Range<T>& other) {
    return from == other.from && to == other.to;
  }

  bool operator==(const T& val) const { return from <= val && val <= to; }

  friend bool operator==(const T& val, const Range<T>& range) {
    return range == val;
  }

  T from, to;
};
