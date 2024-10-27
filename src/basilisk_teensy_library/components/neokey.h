#pragma once

#include <Adafruit_NeoKey_1x4.h>
#include <Arduino.h>

#include <functional>

#include "../globals/serials.h"
#include "../helpers/halt.h"

/* Definition of row, col, x, y in a matrix of NeoKey1x4s:
 *             col 0            1            2
 * row y         x    0 1 2 3      4 5 6 7      8 9 a b
 *   0 0  _neokeys[0]{K K K K} [1]{K K K K} [2]{K K K K}
 *   1 1          [3]{K K K K} [4]{K K K K} [5]{K K K K}
 *   2 2          [6]{K K K K} [7]{K K K K} [8]{K K K K} */

/* A wrapper class of Adafruit_MultiNeoKey1x4, callback handling done right. */
class Neokey : public Adafruit_MultiNeoKey1x4 {
 public:
  Neokey(Adafruit_NeoKey_1x4* neokeys, uint8_t rows, uint8_t cols)
      : Adafruit_MultiNeoKey1x4{neokeys, rows, cols} {
    Wire.begin();
    delay(100);
#if DEBUG_SETUP
    Pln("Neokey: Wire began");
#endif

    if (!begin()) {
      HALT("Neokey: Begin failed");
    }

    prev_pressed_ = new uint8_t[_rows * _cols];
    memset(prev_pressed_, 0, _rows * _cols);

#if DEBUG_SETUP
    Pln("Neokey: Setup complete");
#endif
  }

  ~Neokey() { delete[] prev_pressed_; }

  // Must be called before use.
  void Setup(const std::function<void(uint16_t)>& rise_callback) {
    rise_callback_ = rise_callback;
#if DEBUG_SETUP
    Pln("Neokey: Set rise callback");
#endif
  }

  // Call in regular interval short enough to ensure that no physical press of a
  // button is missed.
  void Read() {
    for (uint8_t row = 0; row < _rows; row++) {
      for (uint8_t col = 0; col < _cols; col++) {
        const uint8_t nk_idx = row * _cols + col;
        auto& nk = _neokeys[nk_idx];

        // "Not sure why we have to do it twice." -- Adafruit
        nk.digitalReadBulk(NEOKEY_1X4_BUTTONMASK);
        auto pressed = nk.digitalReadBulk(NEOKEY_1X4_BUTTONMASK);
        pressed ^= NEOKEY_1X4_BUTTONMASK;
        pressed &= NEOKEY_1X4_BUTTONMASK;
        pressed >>= NEOKEY_1X4_BUTTONA;

        // Compare to last reading.
        auto& prev_pressed = prev_pressed_[nk_idx];
        uint8_t just_pressed = (pressed ^ prev_pressed) & pressed;

        // Call callback for risen buttons.
        for (int btn = 0; btn < 4; btn++) {
          if (just_pressed & (1 << btn)) {
            rise_callback_((nk_idx << 2) + btn);
          }
        }

        // Stash for next run.
        prev_pressed = pressed;
      }
    }
  }

 private:
  std::function<void(uint16_t)> rise_callback_;
  uint8_t* prev_pressed_;
};
