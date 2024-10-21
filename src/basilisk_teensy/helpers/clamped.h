#pragma once

#include <Arduino.h>

#ifndef NaN
#define NaN (0.0 / 0.0)
#endif

template <typename T>
T clamp(const T& val, const T& lb, const T& ub) {
  if (val != val) return NaN;
  if (val < lb) return lb;
  if (val > ub) return ub;
  return val;
}

template <typename T>
class clamped {
 public:
  clamped() : val_{T{0}} {}

  clamped(const clamped& other) { val_ = other.val_; }

  clamped& operator=(const T& new_val) {
    val_ = clamp(new_val, lb(), ub());
    return *this;
  }

  clamped& operator=(const clamped& other) {
    val_ = clamp(other.val_, lb(), ub());
    return *this;
  }

  clamped& operator=(clamped&& other) {
    val_ = clamp(other.val_, lb(), ub());
    return *this;
  }

  operator T() const { return val_; }

  bool isnan() { return val_ != val_; }

 protected:
  T val_;
  virtual T lb() const = 0;
  virtual T ub() const = 0;
};

class Phi : public clamped<double> {
 public:
  Phi(const double& init_val = 0.0) { val_ = clamp(init_val, lb(), ub()); }

  using clamped::operator=;

 private:
  double lb() const final { return -0.3; }
  double ub() const final { return 0.3; }
};

class PhiSpeed : public clamped<double> {
 public:
  PhiSpeed(const double& init_val = 0.0) { val_ = clamp(init_val, lb(), ub()); }

  using clamped::operator=;

 private:
  double lb() const final { return 0.0; }
  double ub() const final { return 1.0; }
};

class PhiAccLim : public clamped<double> {
 public:
  PhiAccLim(const double& init_val = 1.0) {
    val_ = clamp(init_val, lb(), ub());
  }

  using clamped::operator=;

 private:
  double lb() const final { return 0.5; }
  double ub() const final { return 10.0; }
};

class PhiThr : public clamped<double> {
 public:
  PhiThr(const double& init_val = 0.01) { val_ = clamp(init_val, lb(), ub()); }

  using clamped::operator=;

 private:
  double lb() const final { return 0.001; }
  double ub() const final { return 1.0; }
};

class N64 : public clamped<int> {
 public:
  N64(const int& init_val = 32) { val_ = clamp(init_val, lb(), ub()); }

  using clamped::operator=;

 private:
  int lb() const final { return 1; }
  int ub() const final { return 64; }
};
