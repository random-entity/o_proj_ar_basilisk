#include <Xbee3.h>
#include <parasite.h>

xb::Sender s{XBEE_SERIAL};
Neokey& nk = specifics::neokey1x4_i2c0;

uint8_t frame_data_1[1] = {0x00};
uint8_t frame_data_2[2] = {0xAA, 0xBB};
uint8_t frame_data_3[3] = {0xAA, 0xBB, 0xCC};
uint8_t frame_data_4[5] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE};
uint8_t frame_data_5[6] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0x7A};

void setup() {
  InitSerial();

  XBEE_SERIAL.begin(XBEE_SERIAL_BAUDRATE);
  delay(SERIAL_BEGIN_WAIT_TIME_MS);

  nk.Setup([](uint16_t key) {
    Pln("send");
    if (key == 2) {
      if (s.Send(frame_data_4, 5)) Pln("success");
    } else if (key == 3) {
      if (s.Send(frame_data_5, 6)) Pln("success");
    }
  });
}

void loop() {
  nk.Read();
  delay(10);

  static Beat hearbeat{1000};
  static bool high = false;
  if (hearbeat.Hit()) {
    nk.setPixelColor(0, high ? 0xFFFFFF : 0);
    nk.show();
    high = !high;
  }
}
