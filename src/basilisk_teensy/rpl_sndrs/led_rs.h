#pragma once

#include <Arduino.h>

// #include "../cmd_rcvrs/xbee_cr.h"
#include "../components/neokey.h"
#include "../helpers/beat.h"

namespace led_rs {
bool finding_me;
uint8_t suid;
}  // namespace led_rs

void LedReplySender(Neokey& nk) {
  static Beat teensy_hearbeat{125};
  static bool on = true;
  if (teensy_hearbeat.Hit()) {
    // nk.setPixelColor(0, on ? 0x400040 : 0x000000);

    if (led_rs::suid != 13) {
      auto num_hearts = (led_rs::suid - 1) / 3 + 1;
      auto color = led_rs::suid % 3 == 1   ? 0xF00000
                   : led_rs::suid % 3 == 2 ? 0x00F000
                                           : 0x0000F0;
      for (uint8_t i = 0; i < num_hearts; i++) {
        nk.setPixelColor(i, on ? color : 0x000000);
      }
      for (uint8_t i = num_hearts; i < 4; i++) {
        nk.setPixelColor(i, 0x000000);
      }
    } else {
      for (uint8_t i = 0; i < 4; i++) {
        nk.setPixelColor(i, on ? 0xF0F0F0 : 0x000000);
      }
    }

    if (led_rs::finding_me) {
      nk.setPixelColor(3, 0xF0F000);
      led_rs::finding_me = false;
    }

    on = !on;

    nk.show();
  }

  // using XCR = XbeeCommandReceiver;
  // nk.setPixelColor(1, XCR::led_got_start_bytes ? 0x002020 : 0x000000);
  // nk.setPixelColor(2, XCR::got_full_packet ? 0x000020 : 0x000000);
  // nk.setPixelColor(2, XCR::led_got_my_cmd ? 0x002020 : 0x000000);
  // nk.setPixelColor(3, XCR::led_timeout_miss ? 0x800000 : 0x000000);
}
