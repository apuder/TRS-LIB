
#include "inout.h"

void out(uint8_t port, uint8_t val)
{
  __asm
    ld c,a          ; port->a, val->l
    out (c),l
  __endasm;
}

uint8_t in(uint8_t port)
{
  __asm
    ld c,a          ; port->a
    in  a,(c)
  __endasm;
}

void out31(uint8_t val)
{
  __asm
    out (#31),a
  __endasm;
}

uint8_t in31()
{
  __asm
    in  a,(#31)
  __endasm;
}


