#include <Xbee3.h>
#include <parasite.h>

Neokey& nk = specifics::neokey1x4_i2c0;

xb::Sender s{};

void setup() {
  InitSerial();

  nk.Setup([](uint16_t key) {
    Pln("sent");
    
  })
}

void loop() {}
