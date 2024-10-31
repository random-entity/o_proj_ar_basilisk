import java.util.HashMap;

final int xb_start = 0x7E;
final int xb_esc = 0x7D;
final int xb_xon = 0x11;
final int xb_xoff = 0x13;
final int xb_xor_with = 0x20;
final int buf_capacity = 100;

class XbRR {
  XbRR(Serial _port) {
    port = _port;

    srcAddrToSuid.put(0x0013A20041C22F0EL, 1);
    srcAddrToSuid.put(0x0013A20041C22D2AL, 2);
    srcAddrToSuid.put(0x0013A20041C2AE4EL, 3);
    srcAddrToSuid.put(0x0013A20041C2AF68L, 4);
    srcAddrToSuid.put(0x0013A20041C15729L, 5);
    srcAddrToSuid.put(0x0013A20041C22D41L, 6);
    srcAddrToSuid.put(0x0013A20041C1369CL, 7);
    srcAddrToSuid.put(0x0013A20041C2B6D4L, 8);
    srcAddrToSuid.put(0x0013A20041C22F71L, 9);
    srcAddrToSuid.put(0x0013A20041BEA1C1L, 10);
    srcAddrToSuid.put(0x0013A20041C22D99L, 11);
    srcAddrToSuid.put(0x0013A20041C22EFCL, 12);
    srcAddrToSuid.put(0x0013A20041C2F781L, 13);

    srcAddrToSuid.put(0x0013A20041C2A71BL, 12);
  }

  Serial port;
  int[] buf = new int[buf_capacity];
  int idx = 0;
  int size = 0;
  int checksum = 0;
  boolean esc = false;
  int w = 0;  // Waiting what? 0: Start, 1: Length, 2: Checksum
  HashMap<Long, Integer> srcAddrToSuid = new HashMap<>();

  void goToWaitStart() {
    w = 0;
  }
  void goToWaitLength() {
    w = 1;
    idx = 0;
  }
  void goToWaitChecksum() {
    w = 2;
    idx = 0;
    checksum = 0;
    size = buf[0] * 256 + buf[1];
  }

  void run() {
    if (w == 0) {
      while (port.available() > 0) {
        if (port.read() == xb_start) {
          goToWaitLength();
          break;
        }
      }
      if (w == 0) return;
    }

    if (w == 1) {
      while (port.available() > 0) {
        int r = port.read();
        if (r == xb_start) {
          goToWaitLength();
          continue;
        } else {
          if (!put(r, false)) {
            goToWaitStart();
            return;
          }
          if (idx >= 2) {
            goToWaitChecksum();
            if (size < 12) {
              goToWaitStart();
              return;
            }
            break;
          } else {
            continue;
          }
        }
      }
      if (w == 1) return;
    }

    if (w == 2) {
      while (port.available() > 0) {
        int r = port.read();
        if (r == xb_start) {
          goToWaitLength();
          return;
        } else {
          if (!put(r, true)) {
            goToWaitStart();
            return;
          }
          if (idx >= size + 1) {
            if (checksum % 256 == 255 && buf[0] == 0x90) {
              inject();
            }
            goToWaitStart();
            return;
          } else {
            continue;
          }
        }
      }
    }
  }

  boolean put(int val, boolean sum) {
    if (idx >= buf_capacity) {
      return false;
    }

    if (val == xb_esc) {
      esc = true;
      return true;
    }

    val = esc ? val ^ xb_xor_with : val;
    buf[idx++] = val;
    if (sum) checksum += val;
    esc = false;
    return true;
  }

  void inject() {
    Long srcAddr = bigEndianBytesToUint64(buf, 1);

    if (srcAddrToSuid.containsKey(srcAddr)) {
      int suid = srcAddrToSuid.get(srcAddr);
      Basilisk b = bs.get(suid - 1);
      b.phil = littleEndianBytesToFloat(buf, 12);
      b.phir = littleEndianBytesToFloat(buf, 16);
      b.lpsx = littleEndianBytesToFloat(buf, 20);
      b.lpsy = littleEndianBytesToFloat(buf, 24);
      b.yaw = littleEndianBytesToFloat(buf, 28);
      b.lastRplTime = millis();
    }
  }
}
