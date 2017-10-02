#ifndef __RANDOM_H__
#define __RANDOM_H__
#include <stdint.h>

#define STATE_BYTES 7
#define MULT 0x13B /* for STATE_BYTES==6 only */
#define MULT_LO (MULT & 255)
#define MULT_HI (MULT & 256)

static inline
uint8_t getrnd(void)
{
    static uint8_t state[STATE_BYTES] = \
      { 0x87, 0xdd, 0xdc, 0x10, 0x35, 0xbc, 0x5c };
    static uint16_t c = 0x42;
    static int32_t i = 0;
    uint16_t t;
    uint8_t x;

    x = state[i];
    t = (uint16_t)x * MULT_LO + c;
    c = t >> 8;
#if MULT_HI
    c += x;
#endif
    x = t & 255;
    state[i] = x;
    if (++i >= sizeof(state))
        i = 0;
    return x;
}


#endif		/*__RANDOM_H__*/
