
#ifndef __INOUT_H__
#define __INOUT_H__

#ifndef ESP_PLATFORM

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

__sfr __at 0x84 M3_MEM_MAP;

inline void set_m3_mem_map()
{
  // For M4, turn on MIII memory map. Nop on a MIII/M1
  M3_MEM_MAP = 0;
}

#endif
#endif
