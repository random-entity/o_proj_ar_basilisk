#pragma once

#include <elapsedMillis.h>

#include "_meta.h"

void ModeRunners::Pivot() {
  pv.didim_idx = pv.c.didimbal == BOOL_L ? IDX_L : IDX_R;
  pv.kick_idx = pv.c.didimbal == BOOL_L ? IDX_R : IDX_L;

  switch (m) {
    case M::Pivot_Init: {
      // Check if we need to set didimbal.
      if (pv.c.bend[pv.didim_idx].isnan()) {
        m = M::Pivot_Kick;
        return;
      }

      pv.since_init = 0;
      pv.didim_init_yaw = b.yaw();

      // Release didimbal, attach kickbal, set phi_didim.
      m = M::SetMags_Init;
      const bool attach_l = pv.c.didimbal != BOOL_L;
      const bool attach_r = pv.c.didimbal != BOOL_R;
      mg.c.strengths[0] = Bool2MS(attach_l);
      mg.c.strengths[1] = Bool2MS(attach_l);
      mg.c.strengths[2] = Bool2MS(attach_r);
      mg.c.strengths[3] = Bool2MS(attach_r);
      mg.c.expected_state[IDX_L] = attach_l;
      mg.c.expected_state[IDX_R] = attach_r;
      mg.c.verif_thr = 5;
      mg.c.min_dur = 50;
      mg.c.max_dur = 200;
      mg.c.exit_to_mode = M::SetPhis_Init;
      ph.c.tgt_phi[pv.didim_idx] = [this] {
        auto tgt_yaw = pv.c.tgt_yaw();
        if (isnan(tgt_yaw)) tgt_yaw = pv.didim_init_yaw;
        return pv.didim_init_yaw - tgt_yaw - pv.c.bend[pv.didim_idx];
      };
      ph.c.tgt_phispeed[pv.didim_idx] = pv.c.speed;
      ph.c.tgt_phiacclim[pv.didim_idx] = pv.c.acclim;
      ph.c.tgt_phi[pv.kick_idx] = [] { return NaN; };
      ph.c.damp_thr = 0.05;
      ph.c.fix_thr = 0.005;
      ph.c.fixing_cycles_thr = 1;
      ph.c.min_dur = 0;
      ph.c.max_dur = isnan(pv.c.stride()) ? pv.c.max_dur : pv.c.max_dur / 4;
      ph.c.exit_condition = pv.c.exit_condition;
      ph.c.exit_to_mode = M::Pivot_Kick;
    } break;
    case M::Pivot_Kick: {
      // Check if we need to kick.
      if (isnan(pv.c.stride())) {
        m = pv.c.exit_to_mode;
        return;
      }

      pv.kick_init_yaw = b.yaw();
      pv.kick_init_phi_didim = b.phi(pv.didim_idx);

      // Release kickbal, attach didimbal, set phis.
      m = M::SetMags_Init;
      const bool attach_l = pv.c.didimbal == BOOL_L;
      const bool attach_r = pv.c.didimbal == BOOL_R;
      mg.c.strengths[0] = Bool2MS(attach_l);
      mg.c.strengths[1] = Bool2MS(attach_l);
      mg.c.strengths[2] = Bool2MS(attach_r);
      mg.c.strengths[3] = Bool2MS(attach_r);
      mg.c.expected_state[IDX_L] = attach_l;
      mg.c.expected_state[IDX_R] = attach_r;
      mg.c.verif_thr = 5;
      mg.c.min_dur = 50;
      mg.c.max_dur = 200;
      mg.c.exit_to_mode = M::SetPhis_Init;
      ph.c.tgt_phi[pv.didim_idx] = [this] {
        auto tgt_yaw = pv.c.tgt_yaw();
        if (isnan(tgt_yaw)) tgt_yaw = pv.didim_init_yaw;

        const auto sgnd_stride =
            pv.c.stride() * (pv.c.didimbal == BOOL_L ? 1.0 : -1.0);

        return pv.kick_init_phi_didim + tgt_yaw + sgnd_stride -
               pv.kick_init_yaw;
      };
      ph.c.tgt_phi[pv.kick_idx] = [this] {
        const auto sgnd_stride =
            pv.c.stride() * (pv.c.didimbal == BOOL_L ? 1.0 : -1.0);

        return -pv.c.bend[pv.kick_idx] + sgnd_stride;
      };
      ph.c.tgt_phispeed[pv.didim_idx] = pv.c.speed;
      ph.c.tgt_phispeed[pv.kick_idx] = pv.c.speed;
      ph.c.tgt_phiacclim[pv.didim_idx] = pv.c.acclim;
      ph.c.tgt_phiacclim[pv.kick_idx] = pv.c.acclim;
      ph.c.damp_thr = 0.05;
      ph.c.fix_thr = 0.005;
      ph.c.fixing_cycles_thr = 1;
      ph.c.min_dur =
          pv.c.min_dur > pv.since_init ? pv.c.min_dur - pv.since_init : 0;
      ph.c.max_dur =
          pv.c.max_dur > pv.since_init ? pv.c.max_dur - pv.since_init : 0;
      ph.c.exit_condition = pv.c.exit_condition;
      ph.c.exit_to_mode = pv.c.exit_to_mode;
    } break;
    default:
      break;
  }
}
