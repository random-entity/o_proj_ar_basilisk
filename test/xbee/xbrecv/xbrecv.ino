#include <Xbee3.h>
#include <parasite.h>

xb::Receiver r{XBEE_SERIAL, [](uint8_t* payload, int size) {
                 for (int i = 0; i < size; i++) {
                   Serial.printf("%02X\n", payload[i]);
                 }
               }};

void setup() {
  InitSerial();

  XBEE_SERIAL.begin(XBEE_SERIAL_BAUDRATE);
  delay(SERIAL_BEGIN_WAIT_TIME_MS);
}

void loop() { r.Run(); }
