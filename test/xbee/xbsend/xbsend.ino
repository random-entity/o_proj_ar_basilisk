#include <Xbee3.h>
#include <parasite.h>

#define XBEE_SERIAL (Serial4)
#define XBEE_SERIAL_BAUDRATE (115200)

xb::Sender s{XBEE_SERIAL};
Neokey& nk = specifics::neokey1x4_i2c0;

void setup() {
  Serial.begin(9600);
  delay(100);

  XBEE_SERIAL.begin(XBEE_SERIAL_BAUDRATE);
  delay(100);

  nk.Setup([](uint16_t key) {
    Pln("sent");
    s.Send(nullptr, 0xABCD);
  });
}

void loop() {
  nk.Read();
  delay(10);
}
