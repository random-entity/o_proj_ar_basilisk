#include <parasite.h>

void setup() {
  parasite::main(
      {
          .lps{
              .c = 1100.0,
              .x_c = 1100.0,
              .y_c = 700.0,
              .minx = 50.0,
              .maxx = 1050.0,
              .miny = 50.0,
              .maxy = 650.0,
          },
      },
      specifics::neokey1x4_i2c0);
}

void loop() {}
