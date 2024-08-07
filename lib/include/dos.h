
#ifndef __DOS_H__
#define __DOS_H__

#include "defs.h"

typedef union {
  char fname[32];
} fcb_t;

typedef uint8_t buffer_t[256];

typedef uint8_t err_t;

err_t dos_fspec(const char* fn, fcb_t* fcb);
err_t dos_open(fcb_t* fcb, buffer_t* buffer, uint8_t lrl);
void print(const char* fmt, ...);
void init_dos();

#endif
