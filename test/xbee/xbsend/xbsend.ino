#include <Xbee3.h>
#include <parasite.h>

xb::Sender s{XBEE_SERIAL};
Neokey& nk = specifics::neokey1x4_i2c0;

uint8_t frame_data_1[1] = {0x00};
uint8_t frame_data_2[2] = {0xAA, 0xBB};
uint8_t frame_data_3[3] = {0xAA, 0xBB, 0xCC};
uint8_t frame_data_4[5] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE};
uint8_t frame_data_5[6] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0x7A};
uint8_t frame_data_6[10] = {0xAA, 0xBB, 0xCC, 0x7D, 0xDD,
                            0xEE, 0x7E, 0x11, 0x22, 0x13};
uint8_t frame_data_7[10] = {0x7D, 0x7D, 0x7D, 0x7D, 0x7D,
                            0x7D, 0x7D, 0x7D, 0x7D, 0x7D};
uint8_t frame_data_8[41];
uint8_t frame_data_flood[60];

void setup() {
  InitSerial();

  XBEE_SERIAL.begin(XBEE_SERIAL_BAUDRATE);
  delay(COMMON_SERIAL_BEGIN_WAIT_TIME);

  for (int i = 0; i < 40; i++) {
    frame_data_8[i] = xb::c::start;
  }
  frame_data_8[40] = xb::c::xoff;

  for (auto& b : frame_data_flood) {
    b = 0x7D;
  }

  nk.Setup([](uint16_t key) {
    Pln("send");
    if (key == 0) {
      if (s.Send(frame_data_5, 6)) Pln("success");
    } else if (key == 1) {
      if (s.Send(frame_data_6, 10)) Pln("success");
    } else if (key == 2) {
      if (s.Send(frame_data_7, 10)) Pln("success");
    } else if (key == 3) {
      if (s.Send(frame_data_8, 41)) Pln("success");
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
