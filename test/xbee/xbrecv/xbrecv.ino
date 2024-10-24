#include <Xbee3.h>
#include <parasite.h>

xb::Receiver r{XBEE_SERIAL, [](xb::ReceivePacket& packet, int payload_size) {
                 Serial.print("Received packet from address ");
                 Serial.printf("0x%016llX\n", packet.src_addr());
                 Serial.print("Payload -> ");
                 for (int i = 0; i < payload_size; i++) {
                   Serial.printf("%02X ", packet.payload[i]);
                 }
                 Serial.println();
               }};

void setup() {
  InitSerial();

  XBEE_SERIAL.begin(XBEE_SERIAL_BAUDRATE);
  delay(COMMON_SERIAL_BEGIN_WAIT_TIME);
}

void loop() { r.Run(); }
