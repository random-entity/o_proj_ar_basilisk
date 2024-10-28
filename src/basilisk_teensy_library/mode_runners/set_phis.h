#pragma once

#include "_meta.h"

void ModeRunners::SetPhis() {
  static auto& ph = c.set_phis;

  static uint32_t since_init;
  static uint8_t fixing_cycles[2];

  switch (m) {
    case M::SetPhis_Init: {
      since_init = 0;
      for (const auto f : IDX_LR) fixing_cycles[f] = 0;
      m = M::SetPhis_Move;
    } break;
    case M::SetPhis_Move: {
      if ([&] {
            if (since_init > ph.max_dur || ph.exit_condition()) {
              return true;
            }

            for (const auto f : IDX_LR) {
              auto& s = b.s_[f];

              const auto tgt_outpos = ph.tgt_phi[f]();
              double tgt_rtrvel;
              double tgt_rtracclim;

              if (tgt_outpos.isnan()) {
                // Velocity control with no target position is NOT allowed
                // for Basilisk.
                tgt_rtrvel = 0.0;
                tgt_rtracclim = NaN;
                incu8(fixing_cycles[f]);
              } else {
                const auto cur_outpos = b.phi(f);
                const auto tgt_delta_outpos = tgt_outpos - cur_outpos;
                if (abs(tgt_delta_outpos) < ph.fix_thr) {
                  // Target phi is reached.
                  tgt_rtrvel = 0.0;
                  tgt_rtracclim = NaN;
                  incu8(fixing_cycles[f]);
                } else {
                  tgt_rtrvel =
                      b.cfg_.gr * ph.tgt_phispeed[f]() *
                      /* dx/dt = -x^p where x(t=0) = 1 gives:
                       * x = e^{-t} for p = 1
                       * x = (1 - (1 - p)t)^{1 / (1 - p)} elsewhere
                       * x converges to 0 asymptotically if p >= 1,
                       *   rate of convergence decreasing as p increases.
                       * On the other hand, x hits 0 in finite time if p < 1,
                       *   rate of which increases as p decreases.
                       * Therefore, damping is achieved if p > 0. */
                      signedpow(
                          constrain(tgt_delta_outpos / ph.damp_thr, -1.0, 1.0),
                          0.6);
                  tgt_rtracclim = b.cfg_.gr * ph.tgt_phiacclim[f]();
                  fixing_cycles[f] = 0;
                }
              }

              s.SetPosition([&] {
                auto pm_cmd = *b.pm_cmd_template_;
                pm_cmd.position = NaN;
                pm_cmd.velocity = tgt_rtrvel;
                pm_cmd.accel_limit = tgt_rtracclim;
                return pm_cmd;
              }());
            }

            return fixing_cycles[IDX_L] >= ph.fixing_cycles_thr &&
                   fixing_cycles[IDX_R] >= ph.fixing_cycles_thr &&
                   since_init > ph.min_dur;
          }()) {
        m = ph.exit_to_mode;
      }
    } break;
    default:
      break;
  }
}
