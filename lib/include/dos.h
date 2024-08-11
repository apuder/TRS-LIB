
#ifndef __DOS_H__
#define __DOS_H__

#include "defs.h"

#define NO_ERR 0
#define ERR_EOF 28
#define ERR_REC_OUT_OF_RANGE 29
#define ERR_LRL 42

typedef union {
  char fname[48];
} fcb_t;

typedef uint8_t buffer_t[256];
typedef void* urec_t;

typedef struct {
  const char fname[15];
  uint8_t    prot_level;
  uint8_t    eof;
  uint8_t    lrl;
  uint16_t   lst_sec;
  uint16_t   ngran;
} dir_t;

#define MAX_DIR_ENTRIES 81

typedef dir_t dir_buf_t[MAX_DIR_ENTRIES];

typedef uint8_t err_t;

err_t dos_fspec(const char* fn, fcb_t* fcb);
err_t dos_open(fcb_t* fcb, buffer_t* buffer, uint8_t lrl);
err_t dos_init(fcb_t* fcb, buffer_t* buffer, uint8_t lrl);
uint16_t dos_getern(fcb_t* fcb);
void dos_setern(fcb_t* fcb);
err_t dos_read(fcb_t* fcb, urec_t* urec);
err_t dos_close(fcb_t* fcb);
err_t dos_ramdir(uint8_t dec, uint8_t drive, dir_buf_t* buffer);
void init_dos();

#endif
