
#include "dos.h"
#include <stdio.h>
#include <stdarg.h>


extern char print_buf[];

void print(const char* fmt, ...) 
{
  va_list argp;

  va_start(argp, fmt);
  vsprintf(print_buf, fmt, argp);
  va_end(argp);

__asm
  push hl
  push de
  push af
  ld de,#0x401d
  ld hl,#_print_buf
loop:
  ld a,(hl)
  or a
  jr z,stop
  call 0x001b
  inc hl
  jr loop
stop:
  pop af
  pop de
  pop hl
__endasm;
}
