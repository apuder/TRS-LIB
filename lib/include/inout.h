
#ifndef __INOUT_H__
#define __INOUT_H__

#include "defs.h"

__sfr __at 0x1f TRS_IO_PORT;

inline void out31(uint8_t val)
{
  TRS_IO_PORT = val;
}

inline uint8_t in31()
{
  return TRS_IO_PORT;
}

#endif
