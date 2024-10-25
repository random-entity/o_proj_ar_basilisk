## Todo
- 안정적 XBee 통신 확보
  - [X] AP = 2 (API with escape)
  - [X] Mesh hop 불필요
  - Time slot 분배 유지
    - Command packet ID를 offset으로 Time slot 순환
- Auto-moonwalk flag
- 충돌 후 대응
  - 충돌 인식
    - SetPhis Mode의 target과 current rotor velocity의 차이가 지속되면 충돌 판정
      - 복잡한 통계학적 방법 불필요
- Pivot과 그 하위 모드에는 target fields를 function pointer로 둘 필요 없다?
  - SetPhis의 exit_condition에서 충돌 방지를 위해 reinit하는 방법 유지
  - 하지만 WalkToPosInField에서는 필요하지 않나?
- System failure 대응
  - Velocity command 사용중이므로 watchdog timeout 필요
    - [X] Timeout mode는 Stop으로
- XBee 통신 모니터링을 위한 LedRS 활용법
- Extra Query 3개부터는 안 된다?
- Tweak with motor_position.sources.[X].pll_filter_hz config

## Ponder
- Awareness function