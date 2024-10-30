#pragma once

#include <Arduino.h>

#include <functional>
#include <map>

#include "../basilisk.h"
#include "../components/neokey.h"
#include "../helpers/beat.h"
// #include "../cmd_rcvrs/xbee_cr.h"

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

    for (const auto* form : forms_) form->set();

    ColorArray result;
    for (int i = 0; i < num_forms_; i++) {
      result += forms_[i]->ca;
    }
    Show(result);
  }

  struct Form {
    ColorArray ca;
    std::function<void()> set;
  };

  struct Heartbeat : Form {
    Heartbeat(LedReplySender& p) : heartbeat{250} {
      set = [&p, this] {
        const int num_hearts = p.suid_ >= 13 ? 4 : p.suidm1_ / 3 + 1;
        const uint32_t color = p.suid_ >= 13        ? 0x202020
                               : p.suidm1_ % 3 == 0 ? 0x400000
                               : p.suidm1_ % 3 == 1 ? 0x004000
                                                    : 0x000040;
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

  struct BPPPBlip : Form {
    BPPPBlip(LedReplySender& p) {
      set = [&p, this] {
        double brightness = (0.5e6 - p.b_.rpl_.since_xbrx_us.bppp) / (0.5e6);
        brightness = min(0.0, brightness);
        brightness = map(brightness, 0.0, 1.0, 0, 255);
        ca.a[3].r = brightness;
        ca.a[3].b = brightness;
      };
    }
  } bppp_blip{*this};

  inline static constexpr int num_forms_ = 1;

  Form* forms_[num_forms_] = {&heartbeat_};

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
