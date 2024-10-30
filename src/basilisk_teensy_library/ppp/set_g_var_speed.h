#pragma once

#include "_meta.h"

namespace ppp {
void PPP::SetGlobalVarSpeed(int level) {
  g::vars::speed = level == 0   ? g::c::speed::slower
                   : level == 1 ? g::c::speed::slow
                   : level == 2 ? g::c::speed::normal
                   : level == 3 ? g::c::speed::fast
                   : level == 4 ? g::c::speed::faster
                                : g::c::speed::fastest;
}
}  // namespace ppp
