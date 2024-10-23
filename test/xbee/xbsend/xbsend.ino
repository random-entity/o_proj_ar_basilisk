#include <Xbee3.h>
#include <parasite.h>

xb::Sender s{XBEE_SERIAL};
Neokey& nk = specifics::neokey1x4_i2c0;

void setup() {
  InitSerial();

  XBEE_SERIAL.begin(XBEE_SERIAL_BAUDRATE);
  delay(SERIAL_BEGIN_WAIT_TIME_MS);

  nk.Setup([](uint16_t) {
    Pln("send");
    s.Send(nullptr, 0xABCD);
  });
}

void loop() {
  nk.Read();
  delay(10);
}
