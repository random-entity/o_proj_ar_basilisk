#include <parasite.h>

void setup() {
  parasite::main({.lps{
      .c = 880.0,
      .x_c = 590.0,
      .y_c = 440.0,
      .minx = 50.0,
      .maxx = 1010.0,
      .miny = -110.0,
      .maxy = 430.0,
  }});
}

void loop() {}
