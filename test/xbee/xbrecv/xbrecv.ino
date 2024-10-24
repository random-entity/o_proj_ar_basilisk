#include <Xbee3.h>
#include <parasite.h>

xb::Receiver r{XBEE_SERIAL, [](xb::ReceivePacket& packet, int payload_size) {
                 Serial.print("Received packet from address ");
                 Serial.printf("0x%08X", packet.src_addr() >> 32);
                 Serial.printf("%08X\n", packet.src_addr() & 0xFFFFFFFF);
                 Serial.print("Payload -> ");
                 for (int i = 0; i < payload_size; i++) {
                   Serial.printf("%02X ", packet.payload[i]);
                 }
                 Serial.println();
               }};

void setup() {
  InitSerial();

  XBEE_SERIAL.begin(XBEE_SERIAL_BAUDRATE);
  delay(SERIAL_BEGIN_WAIT_TIME_MS);
}

void loop() { r.Run(); }
