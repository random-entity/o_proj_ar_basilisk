## Todo
- 안정적 XBee 통신 확보
  - [X] AP = 2 (API with escape)
  - [X] Mesh hop 불필요
  - [X] Time slot 분배 유지
    - [X] Command packet ID를 offset으로 Time slot 순환
- [X] Auto-moonwalk flag
- [X] 충돌 후 대응
  - [X] 충돌 인식: ControlPositionError 이용
- Pivot과 그 하위 모드에는 target fields를 function pointer로 둘 필요 없다?
  - SetPhis의 exit_condition에서 충돌 방지를 위해 reinit하는 방법 유지
  - 하지만 WalkToPosInField에서는 필요하지 않나?
- System failure 대응
  - [X] Velocity command 사용중이므로 watchdog timeout 필요
    - [X] Timeout mode는 Stop으로
- [X] XBee 통신 모니터링을 위한 LedRS 활용법
- Extra Query 3개부터는 안 된다?
- Tweak with motor_position.sources.[X].pll_filter_hz config

### Misc
- Don't function-ify acclim

## Ponder
- Awareness function