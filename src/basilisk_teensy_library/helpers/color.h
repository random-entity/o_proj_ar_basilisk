#pragma once

#include <Arduino.h>

uint32_t HsvToRgb(float h, float s, float v) {
  float r = 0, g = 0, b = 0;

  if (s == 0) {
    r = g = b = v;
  } else {
    h = fmod(h * 6.0f, 6.0f);     // h를 0.0 ~ 6.0 범위로 조정
    int i = static_cast<int>(h);  // Hue 영역
    float f = h - i;              // 소수 부분
    float p = v * (1 - s);
    float q = v * (1 - s * f);
    float t = v * (1 - s * (1 - f));

    switch (i) {
      case 0:
        r = v;
        g = t;
        b = p;
        break;
      case 1:
        r = q;
        g = v;
        b = p;
        break;
      case 2:
        r = p;
        g = v;
        b = t;
        break;
      case 3:
        r = p;
        g = q;
        b = v;
        break;
      case 4:
        r = t;
        g = p;
        b = v;
        break;
      case 5:
      default:
        r = v;
        g = p;
        b = q;
        break;
    }
  }

  return ((static_cast<uint32_t>(r * 255)) << 16) |
         ((static_cast<uint32_t>(g * 255)) << 8) |
         ((static_cast<uint32_t>(b * 255)) << 0);
}
