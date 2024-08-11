
#include "dos.h"
#include <stdio.h>
#include <stdarg.h>


int putchar(int c)
{
__asm
  push hl
  push de
  ld de,#0x401d
  ld a,l
  call 0x001b
  pop hl
  pop de   ; Pop input parameter passed in HL to DE
__endasm;
}