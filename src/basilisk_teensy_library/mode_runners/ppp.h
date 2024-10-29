#pragma once

#include "../globals/ppp.h"
#include "_meta.h"

std::map<Range<uint16_t>, std::function<void()>>
    ModeRunners::PPP::idx_range_to_runner = {

};

void ModeRunners::PPP() {
  using namespace g::ppp;

  if (pp.c.idx == idx::crmux_xbee) {
  }  //...
  else if (pp.c.idx == range::tibu_foot) {
  } else if (pp.c.idx == range::pivot) {
  }  // ...
}
