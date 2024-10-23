#include <Xbee3.h>
#include <parasite.h>

xb::Receiver r{XBEE_SERIAL};

void setup() {
  InitSerial();

  XBEE_SERIAL.begin(XBEE_SERIAL_BAUDRATE);
  delay(SERIAL_BEGIN_WAIT_TIME_MS);

  Serial.println("entering raw print");

  r.RawPrint();
}

void loop() {}
