#pragma once

#include <Arduino.h>

#include "../globals/serials.h"
#include "../helpers/halt.h"
#include "../helpers/using_moteus.h"

// The following pins are selected for the SeoulOpenMedia T4_CanFD board v1.5.
// Only Buses 1 and 2 work for now.
#define MCP2518FD_CS_BUS1 10
#define MCP2518FD_INT_BUS1 41
#define MCP2518FD_CS_BUS2 9
#define MCP2518FD_INT_BUS2 42
#define MCP2518FD_CS_BUS3 0
#define MCP2518FD_INT_BUS3 43
#define MCP2518FD_CS_BUS4 2
#define MCP2518FD_INT_BUS4 44

// The CAN FD driver (MCP2518FD) objects using the ACAN2517FD Arduino library.
ACAN2517FD canfd_drivers[4] = {{MCP2518FD_CS_BUS1, SPI, MCP2518FD_INT_BUS1},
                               {MCP2518FD_CS_BUS2, SPI, MCP2518FD_INT_BUS2},
                               {MCP2518FD_CS_BUS3, SPI1, MCP2518FD_INT_BUS3},
                               {MCP2518FD_CS_BUS4, SPI1, MCP2518FD_INT_BUS4}};

void (*canfd_isrs[4])() = {
    [] { canfd_drivers[0].isr(); }, [] { canfd_drivers[1].isr(); },
    [] { canfd_drivers[2].isr(); }, [] { canfd_drivers[3].isr(); }};

void InitializeCanFdDriver(const int& bus) {
  static uint8_t imiham = 0;

  if (bus < 1 || bus > 4) {
    HALT("CanFdDriverInitializer: Unknown bus");
  } else {
    if (imiham & (1 << (bus - 1))) return;
    imiham |= (1 << (bus - 1));

    if (bus <= 2) {
      SPI.begin();
#if DEBUG_SETUP
      Pln("CanFdDriverInitializer: SPI began");
#endif
    } else {
      SPI1.begin();
#if DEBUG_SETUP
      Pln("CanFdDriverInitializer: SPI1 began");
#endif
    }
  }

  uint32_t err = 0;
  do {
    err = canfd_drivers[bus - 1].begin(
        [] {
          ACAN2517FDSettings settings{ACAN2517FDSettings::OSC_40MHz,
                                      1000ll * 1000ll, DataBitRateFactor::x1};
          settings.mArbitrationSJW = 2;
          settings.mDriverTransmitFIFOSize = 1;
          settings.mDriverReceiveFIFOSize = 2;

          return settings;
        }(),
        canfd_isrs[bus - 1]);

    if (err) {
#if DEBUG_SETUP
      P("CanFdDriverInitializer: CAN FD driver on bus ");
      Serial.print(bus);
      P(" begin failed, error code 0x");
      Serial.println(err, HEX);
#endif
      delay(1000);
    }
  } while (err);

#if DEBUG_SETUP
  P("CanFdDriverInitializer: CAN FD driver on bus ");
  Serial.print(bus);
  Pln(" started");
#endif
}
