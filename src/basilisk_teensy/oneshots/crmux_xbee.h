#pragma once

#include "_meta.h"

void BasiliskOneshots::CRMuxXbee(Basilisk* b) {
  b->crmux_ = Basilisk::CRMux::Xbee;
  b->cmd_.oneshots &= ~(1 << ONESHOT_CRMuxXbee);
}
