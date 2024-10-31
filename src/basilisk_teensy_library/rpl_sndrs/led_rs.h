#pragma once

#include <Arduino.h>

#include <functional>
#include <map>

#include "../basilisk.h"
#include "../components/neokey.h"
#include "../helpers/beat.h"
#include "../helpers/color.h"

class LedReplySender {
 public:
  LedReplySender(Basilisk& b, Neokey& nk, const uint32_t run_interval = 1)
      : b_{b}, nk_{nk}, beat_{run_interval} {}

  struct Color {
    Color() { u.matome = 0; }

    union {
      uint8_t rgb[3];
      uint32_t matome = 0;
    } u;

    uint8_t& r = u.rgb[2];
    uint8_t& g = u.rgb[1];
    uint8_t& b = u.rgb[0];
    uint32_t Get() const { return u.matome & 0xFFFFFF; }

    Color& operator+=(const Color& other) {
      for (int i = 0; i < 3; i++) u.rgb[i] += other.u.rgb[i];
      return *this;
    }
  };

  struct ColorArray {
    Color a[4];

    ColorArray& operator+=(const ColorArray& other) {
      for (int i = 0; i < 4; i++) a[i] += other.a[i];
      return *this;
    }
  };

  void Show(const ColorArray& ca) {
    for (int i = 0; i < 4; i++) {
      nk_.setPixelColor(i, ca.a[i].Get());
    }
    nk_.show();
  }

  void Run() {
    if (!beat_.Hit()) return;

    ColorArray result;
    for (const auto* form : forms_) {
      form->set();
      result += form->ca;
    }
    Show(result);
  }

  struct Form {
    ColorArray ca;
    std::function<void()> set;
  };

  struct Heartbeat : Form {
    Heartbeat(LedReplySender& p) : heartbeat{500} {
      set = [&p, this] {
        const int num_hearts = p.suid_ >= 13 ? 4 : p.suidm1_ / 3 + 1;
        const uint32_t color = p.suid_ >= 13        ? 0x101010
                               : p.suidm1_ % 3 == 0 ? 0x200000
                               : p.suidm1_ % 3 == 1 ? 0x002000
                                                    : 0x000020;
        static bool high = false;

        if (!heartbeat.Hit()) return;

        high = !high;
        for (int i = 0; i < num_hearts; i++) {
          ca.a[i].u.matome = high ? color : 0;
        }
      };
    }

    Beat heartbeat;
  } heartbeat_{*this};

  struct XbRxBlips : Form {
    XbRxBlips(LedReplySender& p) : heartbeat{125} {
      set = [&p, this] {
        /* B-PPP */ {
          double brightness =
              1.0 - static_cast<double>(p.b_.rpl_.since_xbrx_us.bppp) / (100e3);
          brightness = max(0.0, brightness);
          brightness = map(brightness, 0.0, 1.0, 0.0, 200.0);
          ca.a[3].g = static_cast<uint8_t>(brightness);
        }

        /* B-Poll */ {
          double brightness =
              1.0 - static_cast<double>(p.b_.rpl_.since_xbrx_us.bpoll) / (50e3);
          brightness = max(0.0, brightness);
          brightness = map(brightness, 0.0, 1.0, 0.0, 100.0);
          ca.a[3].b = static_cast<uint8_t>(brightness);
        }

        /* FellowReply */ {
          ca.a[2].u.matome = 0;
          for (int fellow_suidm1 = 0; fellow_suidm1 < 13; fellow_suidm1++) {
            if (fellow_suidm1 == p.suidm1_) continue;

            double brightness =
                1.0 - static_cast<double>(
                          p.b_.rpl_.since_xbrx_us.fellow_rpl(fellow_suidm1)) /
                          (50e3);
            brightness = max(0.0, brightness);
            double hue = static_cast<double>(fellow_suidm1) / 13.0;
            ca.a[2].u.matome += HsvToRgb(hue, 1.0, brightness);
          }
        }

        /* Missing FellowReply */ {
          ca.a[1].u.matome = 0;
          for (int fellow_suidm1 = 0; fellow_suidm1 < 13; fellow_suidm1++) {
            if (fellow_suidm1 == p.suidm1_) continue;

            double brightness =
                p.b_.rpl_.since_xbrx_us.fellow_rpl(fellow_suidm1) > 3000000
                    ? 0.75
                    : 0.0;
            double hue = static_cast<double>(fellow_suidm1) / 13.0;
            ca.a[1].u.matome += HsvToRgb(hue, 1.0, brightness);
          }
        }

        /* (Temp) Boundary collision */ {
          ca.a[1].u.matome = 0;
          for (int fellow_suidm1 = 0; fellow_suidm1 < 13; fellow_suidm1++) {
            if (fellow_suidm1 == p.suidm1_) continue;

            if (p.b_.BoundaryCollision() & (1 << fellow_suidm1)) {
              double hue = static_cast<double>(fellow_suidm1) / 13.0;
              ca.a[1].u.matome += HsvToRgb(hue, 1.0, 1.0);
            }
          }
        }
      };
    }

    Beat heartbeat;
  } xbrx_blips{*this};

  Form* forms_[2] = {&heartbeat_, &xbrx_blips};

  Basilisk& b_;
  const int& suid_{b_.cfg_.suid};
  const int& suidm1_{b_.cfg_.suidm1};
  Neokey& nk_;
  Beat beat_;
};

// namespace led_rs {
// bool finding_me;
// uint8_t suid;
// }  // namespace led_rs
// if (led_rs::finding_me) {
//   nk.setPixelColor(3, 0xF0F000);
//   led_rs::finding_me = false;
// }
// using XCR = XbeeCommandReceiver;
// nk.setPixelColor(1, XCR::led_got_start_bytes ? 0x002020 : 0x000000);
// nk.setPixelColor(2, XCR::got_full_packet ? 0x000020 : 0x000000);
// nk.setPixelColor(2, XCR::led_got_my_cmd ? 0x002020 : 0x000000);
// nk.setPixelColor(3, XCR::led_timeout_miss ? 0x800000 : 0x000000);
