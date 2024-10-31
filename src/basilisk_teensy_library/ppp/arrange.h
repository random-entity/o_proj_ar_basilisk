#pragma once

#include "../globals/geo.h"
#include "_meta.h"

void PPPShooter::ArrangeToGrid() {
  // 9   10   11   12   13
  // 5      6     7      8
  // 1      2     3      4
  static const auto& suid = b.cfg_.suid;

  m = M::WalkToPosInField_Init;
  auto& c = b.cmd_.walk_to_pos_in_field;

  static const uint8_t row = suid <= 4 ? 0 : suid <= 8 ? 1 : 2;
  static const uint8_t col = suid <= 12 ? (suid - 1) % 4 : 4;
  static double x =
      row < 2 ? (b.cfg_.lps.minx * (3 - col) + b.cfg_.lps.maxx * col) / 3.0
              : (b.cfg_.lps.minx * (4 - col) + b.cfg_.lps.maxx * col) / 4.0;
  static double y = (b.cfg_.lps.miny * (2 - row) + b.cfg_.lps.maxy * row) / 2.0;
  c.tgt_pos = Vec2{x, y};
}

void PPPShooter::ArrangeToPyramid() {
  // 7    6   13    4    3
  //    12  11   9   10
  //      8    5    2
  //           1
  static const auto& suid = b.cfg_.suid;

  m = M::WalkToPosInField_Init;
  auto& c = b.cmd_.walk_to_pos_in_field;

  c.tgt_pos = suid == 1    ? Vec2{400, 150}
              : suid == 8  ? Vec2{250, 300}
              : suid == 5  ? Vec2{400, 300}
              : suid == 2  ? Vec2{550, 300}
              : suid == 12 ? Vec2{200, 500}
              : suid == 11 ? Vec2{350, 500}
              : suid == 9  ? Vec2{500, 500}
              : suid == 10 ? Vec2{650, 500}
              : suid == 7  ? Vec2{100, 750}
              : suid == 6  ? Vec2{250, 750}
              : suid == 13 ? Vec2{400, 750}
              : suid == 4  ? Vec2{550, 750}
              : suid == 3  ? Vec2{700, 750}
                           : Vec2{425, 425};
}

void PPPShooter::ArrangeToSoloNChorus() {
  // 7 8  9 10 12
  // 3 4      5 6
  // 1    11    2
  //
  // 13?
  static const auto& suid = b.cfg_.suid;
  m = M::WalkToPosInField_Init;
  auto& c = b.cmd_.walk_to_pos_in_field;
  c.tgt_pos = suid == 1    ? Vec2{150, 250}
              : suid == 2  ? Vec2{750, 250}
              : suid == 3  ? Vec2{150, 500}
              : suid == 4  ? Vec2{300, 500}
              : suid == 5  ? Vec2{550, 500}
              : suid == 6  ? Vec2{750, 500}
              : suid == 7  ? Vec2{150, 750}
              : suid == 8  ? Vec2{300, 750}
              : suid == 9  ? Vec2{450, 750}
              : suid == 10 ? Vec2{600, 750}
              : suid == 12 ? Vec2{750, 750}
              : suid == 11 ? Vec2{425, 125}
                           : Vec2{425, 425};
}

void PPPShooter::ArrangeToCircle148(int senw) {
  // mod 10 == SENW where SUID 1, 9 at

  m = M::WalkToPosInField_Init;
  auto& c = b.cmd_.walk_to_pos_in_field;

  if (b.cfg_.suid <= 8) {
    c.tgt_pos = g::geo::circle148_outer[(b.cfg_.suid - 1 + senw - 1 + 8) % 8];
  } else if (b.cfg_.suid <= 12) {
    c.tgt_pos = g::geo::circle148_outer[(b.cfg_.suid - 9 + senw - 1 + 16) % 8];
  } else {
    c.tgt_pos = g::geo::circle148_center;
  }
}

void PPPShooter::ArrangeToCircle013(int robin) {  // Ignore robin for now.
  m = M::WalkToPosInField_Init;
  auto& c = b.cmd_.walk_to_pos_in_field;
  c.tgt_pos = g::geo::circle013[b.cfg_.suidm1];
}
