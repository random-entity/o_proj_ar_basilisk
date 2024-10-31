#pragma once

#include <Arduino.h>

#include "../helpers/vec2.h"

namespace g::geo {

const Vec2 circle148_center{480.0, 175.0};

const Vec2 circle148_outer[8]{
    {480.0, -125.0}, {725.0, -75.0}, {900.0, 125.0}, {725.0, 275.0},
    {450.0, 425.0},  {175.0, 415.0}, {100.0, 250.0}, {175.0, 0.0},
};

const Vec2 circle148_inner[8]{
    {475.0, 50.0},  {610.0, 125.0}, {700.0, 150.0}, {600.0, 250.0},
    {475.0, 300.0}, {325.0, 300.0}, {275.0, 225.0}, {310.0, 50.0},
};

const Vec2 circle013[13]{
    // suidm1 -> pos

    // SUID 1 ~ 8 (SUIDM1 0 ~ 7)
    {100.0, 225.0},
    {150.0, 350.0},
    {275.0, 425.0},
    {400.0, 425.0},
    {575.0, 425.0},
    {675.0, 275.0},
    {800.0, 175.0},
    {850.0, 75.0},

    // SUID 9 ~ 13 (SUIDM1 8 ~ 12)
    {150.0, 50.0},
    {275.0, -75.0},
    {475.0, -125.0},
    {625.0, -125.0},
    {800.0, -50.0},
};

}  // namespace g::geo
