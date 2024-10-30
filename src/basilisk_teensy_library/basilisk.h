#pragma once

#include <Arduino.h>
#include <elapsedMillis.h>

#include <functional>

#include "components/canfd_drivers.h"
#include "components/imu.h"
#include "components/lego_blocks.h"
#include "components/lps.h"
#include "components/magnets.h"
#include "components/servo.h"
#include "globals/moteus_fmt.h"
#include "globals/serials.h"
#include "globals/teensyid.h"
#include "helpers/beat.h"
#include "helpers/range.h"
#include "helpers/utils.h"
#include "roster.h"

struct ModeRunners;

class Basilisk {
 public:
  /////////////////////
  // Configurations: //

  const struct Configuration {
    int suid  // 1 <= ID of this Basilisk <= 13
        = [] {
            int result = 0xDEAD;
            const auto teensyid = GetTeensyId();
            const auto suid_it = g::teensyid::to_suid.find(teensyid);
            if (suid_it != g::teensyid::to_suid.end()) {
              result = suid_it->second;
            }
#if DEBUG_SETUP
            P("SUID -> ");
            Serial.println(result);
#endif
            return result;
          }();
    int suidm1  // 0 <= (SUID - 1) < 13
        = [&] {
            const auto result = suid - 1;
#if DEBUG_SETUP
            P("SUIDM1 -> ");
            Serial.println(result);
#endif
            return result;
          }();
    struct {
      int id_l = 1, id_r = 2;
      int bus = 1;
    } servo;
    struct {
      double c, x_c, y_c;
      double minx, maxx, miny, maxy;
    } lps;
    struct {
      int pin_l = 23, pin_r = 29;
      uint32_t run_interval = 20;
    } lego;
    struct {
      int pin_la = 3, pin_lt = 4, pin_ra = 5, pin_rt = 6;
      uint32_t run_interval = 100;
    } mags;
    double gr = 21.0;  // delta_rotor = delta_output * gr
    double boundary_collision_thr = 100.0;
    double overlapping_collision_thr = 25.0;
  } cfg_;

  const PmCmd* const pm_cmd_template_;

  /////////////////
  // Components: //

  Servo s_[2];
  Servo& l_{s_[IDX_L]};
  Servo& r_{s_[IDX_R]};
  Lps lps_;
  Imu imu_;
  LegoBlocks lego_;
  Magnets mags_;

  //////////////////
  // Constructor: //

  Basilisk(const Configuration& cfg)
      : cfg_{cfg},
        pm_cmd_template_{&g::moteus_fmt::pm_cmd_template},
        s_{{cfg.servo.id_l, cfg.servo.bus,  //
            &g::moteus_fmt::pm_fmt, &g::moteus_fmt::q_fmt},
           {cfg.servo.id_r, cfg.servo.bus,  //
            &g::moteus_fmt::pm_fmt, &g::moteus_fmt::q_fmt}},
        lps_{cfg.lps.c,    cfg.lps.x_c,  cfg.lps.y_c,  //
             cfg.lps.minx, cfg.lps.maxx, cfg.lps.miny, cfg.lps.maxy},
        imu_{},
        lego_{cfg.lego.pin_l, cfg.lego.pin_r, cfg.lego.run_interval},
        mags_{lego_,                             //
              cfg.mags.pin_la, cfg.mags.pin_lt,  //
              cfg.mags.pin_ra, cfg.mags.pin_rt, cfg.mags.run_interval} {
    if (!(1 <= cfg_.suid && cfg_.suid <= 13)) {
      HALT("Basilisk: Bad SUID");
    }
#if DEBUG_SETUP
    Pln("Basilisk: SUID is in valid range [1, 13]");
#endif

    if (cfg_.suidm1 != cfg_.suid - 1) {
      HALT("Basilisk: Bad SUIDM1");
    }
    Pln("Basilisk: Verified SUIDM1 == SUID - 1");

    CommandBoth([](Servo& s) {
      s.SetStop();
      s.SetQuery();
      s.Print();
    });
#if DEBUG_SETUP
    Pln("Basilisk: Both Servos Stopped, Queried and Printed");
    Pln("Basilisk: All components setup succeeded");
#endif
  }

  ////////////////////////
  // Components runner: //

  void Run() {
    lps_.Run();
    imu_.Run();
    lego_.Run();
    mags_.Run();
  }

  /////////////////////////////////
  // Basilisk Command interface: //

  enum class CRMux : bool { Xbee, Neokey } crmux_ = CRMux::Xbee;

  struct Command {
    // Byte representation ranges.
    struct ByteRepRanges {
      inline static const Range<uint8_t> mode{0, 199};
      inline static const Range<uint8_t> oneshot{200, 207};
      inline static const Range<uint8_t> bpoll{232, 232};
    };

    struct BroadcastedPoll {
      elapsedMicros since_us = static_cast<uint32_t>(1e9);
      uint8_t round_robin = 0;
    } bpoll;

    struct Oneshots {
      // Byte representation (200 + n) corresponds to n-th bit (1 << n)
      // of matome value.
      enum class ByteRep : uint8_t {
        CRMuxXbee = 200,
        SetBaseYaw = 201,

        /* PPP: Parameterized-preset-protocol. */
        BPPP = 202,      // PPP Command received by broadcast with payload being
                         // array of PPP indices for all Basilisks in a single
                         // packet.
        /* XPPP = n, */  // There may be additional PPP CR protocols, and PPP
                         // Oneshots corresponding to it, effectively acting as
                         // equivalent Oneshot.  Distinction in Oneshot byterep
                         // is explicitly exposed in order to let Basilisk know
                         // what payload parse logic the CommandSender has
                         // intended.
      };

      uint8_t ToMask(ByteRep o) const {
        return 1 << (static_cast<uint8_t>(o) -
                     Basilisk::Command::ByteRepRanges::oneshot.from);
      }
      void Add(ByteRep o) { matome |= ToMask(o); }
      void Remove(ByteRep o) { matome &= ~ToMask(o); }
      bool Has(ByteRep o) const { return matome & ToMask(o); }

      uint8_t matome = 0;
    } oneshots;

    struct SetBaseYaw {
      double offset;
    } set_base_yaw;

    struct PPP {
      uint16_t idx;
    } ppp;

    enum class Mode : uint8_t {
      // A child Mode cannot be future-chained after its parent Mode.
      // No loop should be formed in a future-chain.

      /* Idle: Kill everything. Relax.
       * - Stop both Servos, attach all magnets,
       * - then do nothing. */
      Idle_Init = 0,  // -> Idle_Nop
      Idle_Nop = 1,   // .

      /* Wait: Wait for some condition to be met.
       *       Future-chain-able.
       * - Loop until given condition is met,
       * - then exit to designated Mode. */
      Wait = 2,  // -> Exit

      /* Free: Stop and release magnets so you can lift it up from the ground.
       *       Magnets will be reactivated when 3 seconds has passed. Only for
       *       convenience in development or emergency during deployment.
       * - Stop both Servos, release all magnets,
       * - then wait 3 seconds,
       * - then exit to Idle Mode. */
      Free = 3,  // -> Wait -> Idle

      /* SetMags: Control magnets.
       *          Future-chain-able.
       *          Duration will be clamped.
       * - Attach or release individual magnets,
       * - then wait for contact/detachment verification,
       * - then exit to designated Mode. */
      SetMags_Init = 5,  // -> SetMags_Wait
      SetMags_Wait = 6,  // -> Exit

      /* RandomMags: Random Tibutibu. */
      RandomMags_Init = 18,
      RandomMags_Do = 19,

      /* SetPhis: Control Servos to achieve target phis.
       *          Future-chain-able.
       *          Phi and duration will be clamped throughout.
       * - PositionMode-Command Servos continuously with .position
       *   set to NaN, .velocity and .accel_limit set to computed as follows:
       *     tgt_rtrvel = tgt_phi == NaN || abs(tgt_delta_phi) < fix_thr ? 0 :
       *                  gr * tgt_phispeed * (tgt_delta_phi >  damp_thr ?  1 :
       *                                       tgt_delta_phi < -damp_thr ? -1 :
       *                                       tgt_delta_phi / damp_thr);
       *     tgt_rtracclim = gr * tgt_phiacclim;
       *   Fix cycles count is incremented every cycle where tgt_rtrvel == 0
       *   and reset elsewhere. Wait until fix cycles count reaches threshold
       *   for both Servos,
       * - then exit to designated Mode. */
      SetPhis_Init = 7,  // -> SetPhis_Move
      SetPhis_Move = 8,  // -> Exit

      /* Pivot: Pivot one foot (kickbal) about the other (didimbal).
       *        The single fundamental element of all Basilisk movement
       *        except Gee.
       *        Future-chain-able.
       *        IsigD + IphiD = Ipsi = IsigK + IphiK
       *     -> (Tpsi + bD) + ? = Ipsi = IsigK + IphiK  (set didimbal)
       *     -> (Tpsi + bD) + ? = Tpsi +- s = (Tpsi + bK) + ?  (kick)
       *
       * - Attach and fix kickbal, release didimbal, and set phi_didim,
       * - then attach didimbal, release kickbal and set both phis,
       * - then exit to designated Mode.
       * - Phi and duration will be clamped throughout. */
      Pivot_Init = 9,   // -> SetMags -> SetPhis -> Pivot_Kick
      Pivot_Kick = 10,  // -> SetMags -> SetPhis -> Exit

      /* PivSeq: Perform a series of Pivots with time intervals.
       *         Future-chain-able. */
      PivSeq_Init = 11,  // -> PivSeq_Step
      PivSeq_Step = 12,  // -> Pivot -> PivSeq_Step(++step) ~> Exit

      /* Walk: An instance of PivSeq implementing unipedalism. */
      PivSpin = 13,  // -> PivSeq -> Idle

      /* Walk: An instance of PivSeq implementing bipedalism. */
      Walk = 20,  // -> PivSeq -> Idle

      /* Walk Variants: Instances of Walk. */
      WalkToDir = 21,  // -> Walk -> Idle
      WalkToPos = 22,  // -> Walk -> Idle
      Sufi = 23,       // -> Walk -> Idle
      Orbit = 24,
      Diamond = 25,
      RandomWalk = 26,
      BounceWalk_Init = 27,
      BounceWalk_Reinit = 28,
      WalkToPosInField_Init = 29,
      WalkToPosInField_Reinit = 30,  // Necessary for immediate re-Pivot.

      /* Gee: */
      Shear_Init = 100,
      Shear_Move = 101,
      Gee = 102,
    } mode = Mode::Idle_Init;

    struct Wait {
      elapsedMillis since_init;
      std::function<bool()> exit_condition;
      Mode exit_to_mode;
    } wait;

    struct SetMags {
      MagStren strengths[4];

      // [0]: l, [1]: r
      // true: contact, false: detachment
      bool expected_state[2];

      N64 verif_thr;
      uint32_t min_dur, max_dur;

      // Exit condition priority:
      // max_dur > min_dur > lego_verification
      Mode exit_to_mode;
    } set_mags;

    struct RandomMags {
      uint32_t min_phase_dur, max_phase_dur;
      uint32_t dur;
    } random_mags;

    struct SetPhis {
      // [0]: l, [1]: r
      // NaN means fix phi (speed, acclim ignored to achieve immediate fix).
      std::function<Phi()> tgt_phi[2];

      // [0]: l, [1]: r
      std::function<PhiSpeed()> tgt_phispeed[2];

      // [0]: l, [1]: r
      std::function<PhiAccLim()> tgt_phiacclim[2];

      PhiThr damp_thr;
      PhiThr fix_thr;
      uint32_t fixing_cycles_thr;
      uint32_t min_dur, max_dur;
      std::function<bool()> exit_condition;

      // Exit condition priority:
      // (max_dur || exit_condition) > (min_dur && fixed_enough)
      Mode exit_to_mode;
    } set_phis;

    struct Pivot {
      LR didimbal;

      // NaN means yaw at Pivot_Init.
      std::function<double()> tgt_yaw;

      // Forward this much more from tgt_yaw. Negative value manifests as
      // walking backwards.
      // NaN means DON'T kick.
      std::function<double()> stride;

      // [0]: l, [1]: r
      // tgt_sig == tgt_yaw + bend
      // NaN means preserve initial sig for didimbal, initial phi for kickbal.
      Phi bend[2];

      std::function<PhiSpeed()> speed;
      std::function<PhiAccLim()> acclim;

      uint32_t min_dur, max_dur;

      // Passed down to SetPhis.
      std::function<bool()> exit_condition;

      // Exit condition priority:
      // max_dur > exit_condition > min_dur
      Mode exit_to_mode;
    } pivot;

    struct PivSeq {
      // .exit_to_mode will be overwritten by PivSeq to PivSeq_Step.
      std::function<Pivot(uint32_t step)> pivots;

      std::function<uint32_t(uint32_t step)> intervals;

      // Total steps to take counting both feet.
      // Negative value means infinity.
      int steps;

      // This is exit condition evaluated every interval between Pivots.
      // Exit condition evaluated while Pivoting should be set at
      // Pivot::exit_condition.
      std::function<bool()> exit_condition;

      // Exit condition priority:
      // exit_condition > steps
      Mode exit_to_mode;
    } pivseq;

    struct PivSpin {
      LR didimbal;

      // NaN means no destination (thus infinite spin).
      double dest_yaw;

      double exit_thr;
      double stride;
      Phi bend[2];
      std::function<PhiSpeed()> speed;
      std::function<PhiAccLim()> acclim;
      uint32_t min_stepdur, max_stepdur;
      uint32_t interval;
      int steps;
    } piv_spin;

    struct Walk {
      LR init_didimbal;
      std::function<double()> tgt_yaw[2];       // [0]: l, [1]: r (didimbal)
      std::function<double()> stride[2];        // [0]: l, [1]: r (didimbal)
      Phi bend[2];                              // [0]: l, [1]: r (didimbal)
      std::function<PhiSpeed()> speed[2];       // [0]: l, [1]: r (didimbal)
      std::function<PhiAccLim()> acclim[2];     // [0]: l, [1]: r (didimbal)
      uint32_t min_stepdur[2], max_stepdur[2];  // [0]: l, [1]: r (didimbal)
      uint32_t interval[2];                     // [0]: l, [1]: r (didimbal)
      int steps;                                // Counting both feet.
      std::function<bool()> exit_condition;  // Passed down to PivSeq AND Pivot.
      Mode exit_to_mode;
    } walk;

    struct WalkToDir {
      LR init_didimbal;

      // Set to true to make Basilisk to automatically walk backwards if target
      // yaw is more than 0.25 revolution away from current yaw.  Current
      // implementation of auto moonwalk will only work for 0 < stride < 0.25.
      bool auto_moonwalk;

      // The direction Basilisk to move to, regardless of moonwalk.
      // NaN means yaw at WalkToDir initialization.
      std::function<double()> tgt_yaw;

      // Use 0 < stride < 0.25 for now due to auto-moonwalk implementation.
      std::function<double()> stride;

      Phi bend[2];
      std::function<PhiSpeed()> speed;
      std::function<PhiAccLim()> acclim;

      // Minimum and maximum duration of all step pivots in milliseconds.
      uint32_t min_stepdur, max_stepdur;

      // Time interval between steps in milliseconds.
      uint32_t interval;

      // Total steps counting both feet. Negative value means infinity.
      int steps;

      std::function<bool()> exit_condition;
      Mode exit_to_mode;
    } walk_to_dir;

    struct WalkToPos {
      LR init_didimbal;
      Vec2 tgt_pos;
      double dist_thr;
      double stride;
      Phi bend[2];
      PhiSpeed speed;
      PhiAccLim acclim;
      uint32_t min_stepdur, max_stepdur;
      uint32_t interval;
      uint8_t steps;
    } walk_to_pos;

    struct Sufi {
      LR init_didimbal;
      double dest_yaw;  // NaN means no destination.
      double exit_thr;
      double stride;
      Phi bend[2];
      PhiSpeed speed;
      PhiAccLim acclim;
      uint32_t min_stepdur, max_stepdur;
      uint32_t interval;
      uint8_t steps;
    } sufi;

    struct Orbit {
      Vec2 center;
      double radius;
    } orbit;

    struct Diamond {
      LR init_didimbal;
      double init_stride;
      PhiSpeed speed;
      PhiAccLim acclim;
      uint32_t min_stepdur, max_stepdur;
      uint32_t interval;
      uint8_t steps;
    } diamond;

    struct BounceWalk {
      double init_tgt_yaw;
    } bounce_walk;

    struct WalkToPosInField {
      Vec2 tgt_pos;
    } walk_to_pos_in_field;

    struct Shear {
      AnkToe fix_which;
      Phi tgt_phi;
    };

    struct Gee {
      friend struct ModeRunners;

     public:
      Gee() {}
      Gee(const double& _stride, const uint8_t& _steps)
          : stride{_stride}, steps{_steps} {}

      // Delta sigma between zero pose and shear pose.
      // Negative value manifests as moving left, and positive right.
      double stride = 0.125;
      // Total steps counting both ankle shears and toe shears.
      uint8_t steps = 8;
      // false = attach ankle, true = attach toe.
      bool phase = false;

     private:
      uint8_t current_step = 0;
    } gee;
  } cmd_;

  ///////////////////////////////
  // Basilisk Reply interface: //

  // It is ReplySender's responsibility to type cast each item.
  struct Reply {
    Reply(Basilisk& _b) : b{_b}, failure{.b{_b}}, since_xbrx_us{.b{_b}} {}

    Basilisk& b;
    const int& suid{b.cfg_.suid};
    const int& suidm1{b.cfg_.suidm1};
    uint8_t mode() { return static_cast<uint8_t>(b.cmd_.mode); }
    double phi_l() { return b.l_.GetReply().abs_position; };
    double phi_r() { return b.r_.GetReply().abs_position; }
    double phi(int f) { return f % 2 == IDX_L ? phi_l() : phi_r(); }
    double lpsx() { return b.lps_.x_; }
    double lpsy() { return b.lps_.y_; }
    uint32_t lps_since_raw_update() { return b.lps_.since_raw_update_; }
    double yaw() { return b.imu_.GetYaw(true); }
    uint32_t imu_since_update() { return b.imu_.since_update_; }

    struct {
      Basilisk& b;
      uint8_t servo_l() { return b.l_.failure_.Export(); }
      uint8_t servo_r() { return b.r_.failure_.Export(); }
      uint8_t heavenfall() {
        uint8_t result = 0;
        for (int i = 0; i < 4; i++) {
          if (b.mags_.heavenfall_[i]) result |= (1 << i);
        }
        return result;
      }
    } failure;

    struct {
      Basilisk& b;
      elapsedMicros bppp;
      const elapsedMicros& bpoll{b.cmd_.bpoll.since_us};
      const elapsedMicros& fellow_rpl(int suidm1) {
        return roster[suidm1].since_update_us;
      }
    } since_xbrx_us;
  } rpl_{*this};

  ///////////////////
  // Util methods: //

  void CommandBoth(std::function<void(Servo&)> c) {
    for (auto& s : s_) c(s);
  }

  uint16_t BoundaryCollision() {
    uint16_t collision = 0;

    const auto my_pos = lps_.GetPos();

    for (uint8_t other_suid = 1; other_suid <= 13; other_suid++) {
      if (other_suid == cfg_.suid) continue;

      const auto& other = roster[other_suid - 1];
      const auto other_pos = Vec2{other.x, other.y};

      if ((other_pos - my_pos).mag() < cfg_.boundary_collision_thr) {
        collision |= (1 << (other_suid - 1));
      }
    }

    return collision;
  }

  void QPrint() {
    CommandBoth([](Servo& s) {
      s.SetQuery();
      s.Print();
    });
  }
};
