#pragma once

#include <Arduino.h>

#ifndef NaN
#define NaN (0.0 / 0.0)
#endif

struct Vec2 {
  double x, y;

  Vec2() : x{0.0}, y{0.0} {}

  Vec2(const double& _x, const double& _y) : x{_x}, y{_y} {}

  Vec2(const double& arg) : x{cos(TWO_PI * arg)}, y{sin(TWO_PI * arg)} {}

  double mag() const { return sqrt(sq(x) + sq(y)); }

  double arg() const { return atan2(y, x) / TWO_PI; }

  double dist(const Vec2& other) const {
    return sqrt(sq(x - other.x) + sq(y - other.y));
  }

  Vec2 operator+(const Vec2& other) const {
    return Vec2{x + other.x, y + other.y};
  }

  Vec2 operator-(const Vec2& other) const {
    return Vec2{x - other.x, y - other.y};
  }

  Vec2 operator*(const double& scalar) const {
    return Vec2{x * scalar, y * scalar};
  }

  friend Vec2 operator*(const double& scalar, const Vec2& vec) {
    return vec * scalar;
  }

  Vec2 operator/(const double& scalar) const {
    if (scalar == 0.0) return Vec2{NaN, NaN};
    return Vec2{x / scalar, y / scalar};
  }

  void add(const Vec2& other) {
    x += other.x;
    y += other.y;
  }

  void sub(const Vec2& other) {
    x -= other.x;
    y -= other.y;
  }

  void scale(const double& scalar) {
    x *= scalar;
    y *= scalar;
  }

  Vec2 normalize() const {
    const auto m = mag();
    if (m == 0.0 || isnan(m)) return Vec2{1.0, 0.0};
    return *this / m;
  }

  double argsub(const Vec2& other) const { return arg() - other.arg(); }
};
