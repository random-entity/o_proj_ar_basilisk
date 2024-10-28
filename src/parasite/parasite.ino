#include <parasite.h>

void setup() {
  parasite::main({.lps{
                     .c = 1100.0,
                     .x_c = 550.0,
                     .y_c = 700.0,
                     .minx = 60.0,
                     .maxx = 1100.0 - 100.0,
                     .miny = 75.0,
                     .maxy = 700.0 - 85.0,
                 }},
                 specifics::neokey1x4_i2c0);
}

void loop() {}
