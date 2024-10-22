#pragma once

#include <Arduino.h>
#include <elapsedMillis.h>

#include "components/canfd_drivers.h"
#include "components/imu.h"
#include "components/lego_blocks.h"
#include "components/lps.h"
#include "components/magnets.h"
#include "components/servo.h"
#include "globals/moteus_fmt.h"
#include "globals/serials.h"
#include "helpers/beat.h"
#include "helpers/serial_print.h"
#include "helpers/teensyid.h"
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
            int suid = 0xDEAD;
            const auto teensyid = GetTeensyId();
            const auto maybe_suid = teensyid_to_suid.find(teensyid);
            if (maybe_suid != teensyid_to_suid.end()) {
              suid = maybe_suid->second;
            }
#if DEBUG_TEENSYID
            InitSerial();
            P("SUID -> ");
            Serial.println(suid);
#endif
            return suid;
          }();
    int suidm1() const {  // 0 <= (SUID - 1) < 13
      static const auto suidm1 = suid - 1;
      return suidm1;
    }
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
    double gr = 21.0;  // delta_rotor = delta_output * gear_ratio
    double collision_thr = 100.0;
    double overlap_thr = 50.0;
  } cfg_;

  const PmCmd* const pm_cmd_template_;

  /////////////////
  // Components: //

  Servo s_[2];
  Servo& l_;
  Servo& r_;
  Lps lps_;          // Run every loop().
  Imu imu_;          // Run every loop().
  LegoBlocks lego_;  // Run in regular interval.
  Magnets mags_;     // Run in regular interval.
  elapsedMicros poll_clk_us_ = 1000000000;

  //////////////////
  // Constructor: //

  Basilisk(const Configuration& cfg)
      : cfg_{cfg},
        pm_cmd_template_{&moteus_fmt::pm_cmd_template},
        s_{{cfg.servo.id_l, cfg.servo.bus,  //
            &moteus_fmt::pm_fmt, &moteus_fmt::q_fmt},
           {cfg.servo.id_r, cfg.servo.bus,  //
            &moteus_fmt::pm_fmt, &moteus_fmt::q_fmt}},
        l_{s_[0]},
        r_{s_[1]},
        lps_{cfg.lps.c,    cfg.lps.x_c,  cfg.lps.y_c,  //
             cfg.lps.minx, cfg.lps.maxx, cfg.lps.miny, cfg.lps.maxy},
        imu_{},
        lego_{cfg.lego.pin_l, cfg.lego.pin_r, cfg.lego.run_interval},
        mags_{lego_,                             //
              cfg.mags.pin_la, cfg.mags.pin_lt,  //
              cfg.mags.pin_ra, cfg.mags.pin_rt, cfg.mags.run_interval},
        rpl_{.b = this,
             .suid = static_cast<uint8_t>(cfg.suid),
             .mode = &cmd_.mode,
             .lpsx = &lps_.x_,
             .lpsy = &lps_.y_} {}

  ////////////////////////////////////////////////////////////
  // Setup method (should be called in setup() before use): //

  bool Setup() {
    if (!(1 <= cfg_.suid && cfg_.suid <= 13)) {
#if DEBUG_SETUP
      Pln("Basilisk: Bad SUID");
#endif
      return false;
    }

    if (!InitializeCanFdDriver(cfg_.servo.bus)) {
#if DEBUG_SETUP
      Pln("Basilisk: CanFdDriver setup failed");
#endif
      return false;
    }
#if DEBUG_SETUP
    Pln("Basilisk: CanFdDriver setup done");
#endif

    CommandBoth([](Servo& s) {
      s.SetStop();
      s.SetQuery();
      s.Print();
    });
#if DEBUG_SETUP
    Pln("Basilisk: Both Servos Stopped, Queried and Printed");
#endif

    if (!lps_.Setup()) {
#if DEBUG_SETUP
      Pln("Basilisk: LPS setup failed");
#endif
      return false;
    }

    if (!imu_.Setup()) {
#if DEBUG_SETUP
      Pln("Basilisk: IMU setup failed");
#endif
      return false;
    }

    if (!lego_.Setup()) {
#if DEBUG_SETUP
      Pln("Basilisk: LegoBlocks setup failed");
#endif
      return false;
    }

    if (!mags_.Setup()) {
#if DEBUG_SETUP
      Pln("Basilisk: Magnets setup failed");
#endif
      return false;
    }

#if DEBUG_SETUP
    Pln("Basilisk: All components setup succeeded");
#endif
    return true;
  }

  ////////////////////////
  // Components runner: //

  void Run() {
    lps_.Run();
    imu_.Run();
    lego_.Run();
    mags_.Run();
  }

  //////////////////////////////
  // Basilisk Command struct: //

  enum class CRMux : bool { Xbee, Neokey } crmux_ = CRMux::Xbee;

  struct Command {
    uint8_t oneshots;  // Refer to utils.h for bit mapping.
                       // Poll and Reply are handled independent of Executer.

    struct SetBaseYaw {
      double offset;
    } set_base_yaw;

    struct Inspire {
    } inspire;

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

      /* DoPreset: Do a preset. */
      DoPreset = 4,

      /* SetMags: Control magnets.
       *          Future-chain-able.
       * - Attach or release individual magnets,
       * - then wait for contact/detachment verification,
       * - then exit to designated Mode.
       * - Duration will be clamped. */
      SetMags_Init = 5,  // -> SetMags_Wait
      SetMags_Wait = 6,  // -> Exit

      /* RandomMags: Randomly tap-dance. */
      RandomMags_Init = 18,
      RandomMags_Do = 19,

      /* SetPhis: Control Servos to achieve target phis.
       *          Future-chain-able.
       * - PositionMode-Command Servos continuously with .position
       *   set to NaN, .velocity and .accel_limit set to computed as follows:
       *     tgt_rtrvel = tgt_phi == NaN || abs(tgt_delta_phi) < fix_thr ? 0 :
       *                  21 * tgt_phispeed * (tgt_delta_phi >  damp_thr ?  1 :
       *                                       tgt_delta_phi < -damp_thr ? -1 :
       *                                       tgt_delta_phi / damp_thr);
       *     tgt_rtracclim = 21 * tgt_phiacclim;
       *   Fix cycles count is incremented every cycle where tgt_rtrvel == 0
       *   and reset elsewhere. Wait until fix cycles count reaches threshold
       *   for both Servos,
       * - then exit to designated Mode.
       * - Phi and duration will be clamped throughout. */
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
      Shear_Init = 250,
      Shear_Move = 251,
      Gee = 252,
    } mode = Mode::Idle_Init;

    struct DoPreset {
      uint16_t idx;
      Mode prev_mode;
    } do_preset;

    struct Wait {
      uint32_t init_time;
      bool (*exit_condition)(Basilisk*);
      Mode exit_to_mode;
    } wait;

    struct SetMags {
      MagStren strengths[4];
      bool expected_state[2];     // [0]: l, [1]: r
                                  // true: contact, false: detachment
      N64 verif_thr;              // Exit condition priority:
      uint32_t min_dur, max_dur;  // max_dur > min_dur > lego_verification
      Mode exit_to_mode;
    } set_mags;

    struct RandomMags {
      uint32_t min_phase_dur, max_phase_dur;
      uint32_t dur;
    } random_mags;

    struct SetPhis {
      Phi tgt_phi[2];  // [0]: l, [1]: r
                       // NaN means fix phi (speed and acclim ignored).
      PhiSpeed (*tgt_phispeed[2])(Basilisk*);  // [0]: l, [1]: r
      PhiAccLim tgt_phiacclim[2];              // [0]: l, [1]: r
      PhiThr damp_thr;
      PhiThr fix_thr;
      uint8_t fixing_cycles_thr;          // Exit condition priority:
      uint32_t min_dur, max_dur;          // (max_dur || exit_condition)
      bool (*exit_condition)(Basilisk*);  // > (min_dur && fixed_enough)
      Mode exit_to_mode;
    } set_phis;

    struct Pivot {
      LR didimbal;                   // Foot to pivot about.
      double (*tgt_yaw)(Basilisk*);  // Evaluated at Pivot_Init
                                     // and used throughout Pivot.
                                     // NaN means yaw at Pivot_Init.
      // (*.*) oO(Ignore me...)
      double stride;   // Forward this much more from tgt_yaw.
                       // Negative value manifests as walking backwards.
                       // NaN means do NOT kick.
      Phi bend[2];     // [0]: l, [1]: r
                       // tgt_sig == tgt_yaw + bend
                       // or bend == -tgt_phi (at stride 0)
                       // NaN means preserve initial sig for didimbal,
                       // initial phi for kickbal.
      PhiSpeed speed;  // EMERGENCY CHANGE ahead of real performance:
                       // Pivot::speed will be ignored and NOT be passed down
                       // to SetPhis. SetPhis::tgt_phispeed will always be
                       // the value of globals::var::speed.
      PhiAccLim acclim;
      uint32_t min_dur, max_dur;
      bool (*exit_condition)(Basilisk*);  // Passed down to SetPhis.
                                          // Exit condition priority:
                                          // max_dur > exit_condition > min_dur
      Mode exit_to_mode;
    } pivot;

    struct PivSeq {
      Pivot (*pivots)(Basilisk*, int);  // exit_to_mode will be
                                        // overwritten by PivSeq.
      uint32_t (*intervals)(Basilisk*, int);
      int steps;                          // Counting both feet.
      bool (*exit_condition)(Basilisk*);  // This is exit condition
                                          // evaluated every interval
                                          // between Pivots. Exit condition
                                          // while Pivoting should be set
                                          // at Pivot::exit_condition.
                                          // Exit condition priority:
                                          // exit_condition > steps
      Mode exit_to_mode;
    } pivseq;

    struct PivSpin {
      LR didimbal;
      double dest_yaw;  // NaN means no destination.
      double exit_thr;
      double stride;
      Phi bend[2];
      PhiSpeed speed;
      PhiAccLim acclim;
      uint32_t min_stepdur, max_stepdur;
      uint32_t interval;
      uint8_t steps;
    } piv_spin;

    struct Walk {
      LR init_didimbal;
      double (*tgt_yaw[2])(Basilisk*);          // [0]: l, [1]: r (didimbal)
      double (*stride[2])(Basilisk*);           // [0]: l, [1]: r (didimbal)
      Phi bend[2];                              // [0]: l, [1]: r (didimbal)
      PhiSpeed speed[2];                        // [0]: l, [1]: r (didimbal)
      PhiAccLim acclim[2];                      // [0]: l, [1]: r (didimbal)
      uint32_t min_stepdur[2], max_stepdur[2];  // [0]: l, [1]: r (didimbal)
      uint32_t interval[2];                     // [0]: l, [1]: r (didimbal)
      uint8_t steps;                            // Counting both feet.
      bool (*exit_condition)(Basilisk*);  // Passed down to PivSeq AND Pivot.
      Mode exit_to_mode;
    } walk;

    struct WalkToDir {
      LR init_didimbal;
      double tgt_yaw;  // NaN means yaw at WalkToDir initialization.
      double stride;
      Phi bend[2];
      PhiSpeed speed;
      PhiAccLim acclim;
      uint32_t min_stepdur, max_stepdur;
      uint32_t interval;
      uint8_t steps;
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

  struct Reply {
    Basilisk* b;
    const uint8_t suid;
    Command::Mode* mode;
    double* lpsx;
    double* lpsy;
    double yaw() { return b->imu_.GetYaw(true); }
    double phi_l() { return b->l_.GetReply().abs_position; }
    double phi_r() { return b->r_.GetReply().abs_position; };

    bool notnull() { return !!b && !!suid && !!mode && !!lpsx && !!lpsy; }
  } rpl_;

  ///////////////////
  // Util methods: //

  template <typename ServoCommand>
  void CommandBoth(ServoCommand c) {
    for (auto& s : s_) c(s);
  }

  uint16_t BoundaryCollision() {
    uint16_t collision = 0;

    const auto my_pos = lps_.GetPos();

    for (uint8_t other_suid = 1; other_suid <= 13; other_suid++) {
      if (other_suid == cfg_.suid) continue;

      const auto& other = roster[other_suid - 1];
      const auto other_pos = Vec2{other.x, other.y};

      if ((other_pos - my_pos).mag() < cfg_.collision_thr) {
        collision |= (1 << (other_suid - 1));
      }
    }

    return collision;
  }

  void Print() {
    CommandBoth([](Servo& s) {
      s.SetQuery();
      s.Print();
    });
  }
};
