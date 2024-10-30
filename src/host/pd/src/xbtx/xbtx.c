#include <stdbool.h>

#include "m_pd.h"

#define BUFFER_CAPACITY (100)

#define XB_START (0x7E)
#define XB_ESC (0x7D)
#define XB_XON (0x11)
#define XB_XOFF (0x13)

#define XB_XOR_WITH (0x20)
#define XB_FRAMETYPE_TX (0x10)
#define XB_ARRD_BROADCAST (0xFFFF);

static t_class* xbtx_class;

typedef struct _xbtx {
  t_object o;
  t_outlet* out;
  t_atom buf[BUFFER_CAPACITY];
} t_xbtx;

// Little endian.
union {
  uint64_t n;
  uint8_t bytes[8];
} ui64;
union {
  uint16_t n;
  uint8_t bytes[2];
} ui16;

bool Put(t_atom* buf, int* idx, uint8_t val, uint8_t* sum) {
  if (val == XB_START || val == XB_ESC || val == XB_XON || val == XB_XOFF) {
    if (*idx + 1 >= BUFFER_CAPACITY) return false;
    SETFLOAT(&buf[*idx], XB_ESC);
    *idx = (*idx) + 1;
    SETFLOAT(&buf[*idx], val ^ XB_XOR_WITH);
    *idx = (*idx) + 1;
  } else {
    if (*idx >= BUFFER_CAPACITY) return false;
    SETFLOAT(&buf[*idx], val);
    *idx = (*idx) + 1;
  }
  if (sum != NULL) *sum += val;
  return true;
}

void xbtx_listin(t_xbtx* x, t_symbol* s, int argc, t_atom* argv) {
  int idx = 1;
  uint8_t sum = 0;
  uint16_t length = (uint16_t)(argc + 14);

  ui16.n = length;

  // Length Bytes
  for (int b = 1; b >= 0; b--) {
    if (!Put(x->buf, &idx, ui16.bytes[b], NULL)) return;
  }

  // Frame Type
  if (!Put(x->buf, &idx, XB_FRAMETYPE_TX, &sum)) return;

  // Frame ID (Unused)
  if (!Put(x->buf, &idx, 0, NULL)) return;  // Suppress response frame.

  // Destination Address
  ui64.n = XB_ARRD_BROADCAST;
  for (int b = 7; b >= 0; b--) {
    if (!Put(x->buf, &idx, ui64.bytes[b], &sum)) return;
  }

  // The meaningless zone
  if (!Put(x->buf, &idx, 0xFF, &sum)) return;
  if (!Put(x->buf, &idx, 0xFE, &sum)) return;

  // Tranmission Configuration
  if (!Put(x->buf, &idx, 0, NULL))
    return;  // Use NH value for broadcast radius.
  if (!Put(x->buf, &idx, 0, NULL))
    return;  // Use TO value for transmit options.

  // Ahh.. finally thehe payload
  for (int i = 0; i < argc; i++) {
    if (argv[i].a_type == A_FLOAT) {
      uint8_t byte = (uint8_t)argv[i].a_w.w_float;
      if (!Put(x->buf, &idx, byte, &sum)) return;
    }
  }

  // Checksum
  if (!Put(x->buf, &idx, 0xFF - sum, NULL)) return;

  // Output.
  outlet_list(x->out, &s_list, idx, x->buf);
}

void* xbtx_new(void) {
  t_xbtx* x = (t_xbtx*)pd_new(xbtx_class);

  SETFLOAT(&x->buf[0], XB_START);
  x->out = outlet_new(&x->o, &s_list);

  return (void*)x;
}

void xbtx_free(t_xbtx* x) {}

void xbtx_setup(void) {
  xbtx_class = class_new(gensym("xbtx"), (t_newmethod)xbtx_new,
                         (t_method)xbtx_free, sizeof(t_xbtx), CLASS_DEFAULT, 0);

  class_addlist(xbtx_class, (t_method)xbtx_listin);
}
