#include <Xbee3.h>

#define XBEE_SERIAL (Serial4)
#define XBEE_SERIAL_BAUDRATE (115200)

xb::Receiver r{XBEE_SERIAL};

void setup() {
  Serial.begin(9600);
  delay(100);

  XBEE_SERIAL.begin(XBEE_SERIAL_BAUDRATE);
  delay(100);

  Serial.println("entering raw print");

  r.RawPrint();
}

void loop() {}
