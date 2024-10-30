#pragma once

#include <Arduino.h>

uint32_t HsvToRgb(float h, float s, float v) {
  float r = 0, g = 0, b = 0;

  int i = int(h * 6.0);   // sector 0 to 5
  float f = h * 6.0 - i;  // fractional part of h
  float p = v * (1.0 - s);
  float q = v * (1.0 - f * s);
  float t = v * (1.0 - (1.0 - f) * s);

  switch (i % 6) {
    case 0:
      r = v, g = t, b = p;
      break;
    case 1:
      r = q, g = v, b = p;
      break;
    case 2:
      r = p, g = v, b = t;
      break;
    case 3:
      r = p, g = q, b = v;
      break;
    case 4:
      r = t, g = p, b = v;
      break;
    case 5:
      r = v, g = p, b = q;
      break;
  }

  uint8_t R = uint8_t(r * 255.0);
  uint8_t G = uint8_t(g * 255.0);
  uint8_t B = uint8_t(b * 255.0);

  return (uint32_t(R) << 16) | (uint32_t(G) << 8) | uint32_t(B);
}
