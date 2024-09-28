
; Reverse engineered from LDOS rename implementation in sys6/sys
    .module dos
    .optsdcc -mz80

    .area _CODE   


_fspec .equ 0x441c
_logot .equ 0x428a
_error .equ 0x4409
_open  .equ 0x4424
_dirrd .equ 0x4b10
_dirwr .equ 0x4b1f
_abort .equ 0x4030
_dircyl .equ 0x4b64
_rdssec .equ 0x4b45
_wrprot .equ 0x4768
_versec .equ 0x4772

fspec_to .equ 0x51EA

  .globl __renam
__renam:              
  LD       (label17 + 1),SP
  push     hl
  ex       de,hl
  LD       DE,#fcb_from
  CALL     _fspec
  pop      hl
  ret      nz
;  JP       NZ,err
  LD       A,(DE)
  CP       #'*'
  JP       Z,err
  LD       DE,#fcb_to
  CALL     _fspec
  ret      nz
  LD       A,(fcb_to)
  CP       #0x0E
  JP       C,err2
  LD       HL,#fcb_from
  LD       DE,#fcb_to
  CALL     sub2
  LD       DE,#fcb_from
  CALL     _open
  RET      NZ
  LD       A,(fcb_from + 1)
  AND      #7
  CP       #3
  LD       A,#'%'
  JP       NC,err4
  LD       HL,#fcb_to

label7:
  LD       A,(HL)
  INC      HL
  CP       #0x0D
  JR       Z,label6
  CP       #0x03
  JR       Z,label6
  CP       #':'
  JR       NZ,label7
  LD       A,(fcb_from + 6)
  XOR      (HL)
  AND      #0x07
  JP       NZ,label8
  JR       label9

label6:
  DEC      HL
  LD       (HL),#':'
  INC      HL
  LD       A,(fcb_from + 6)
  AND      #0x07
  ADD      A,#'0'
  LD       (HL),A
  INC      HL
  LD       (HL),#0x0D

label9:
  LD       A,(0x51E1)      ;???
  PUSH     AF
  LD       DE,#fcb_to
  CALL     _open
  JP       Z,err2
  CP       #0x18
  JP       NZ,err4
  POP      AF
  LD       (0x51E1),A      ;???
  LD       BC,(fcb_from + 6)
  PUSH     BC
  CALL     _dirrd
  JP       NZ,err4
  LD       D,H
  LD       A,L
  ADD      A,#0x05
  LD       E,A
  LD       HL,#fspec_to
  LD       BC,#11
  LDIR
  POP      BC
  CALL     _dirwr
  JP       NZ,err4
  CALL     uptdir
  JP       NZ,err4
  LD       D,H
  LD       E,B
  PUSH     BC
  LD       HL,#fspec_to
  CALL     sub4
  POP      BC
  LD       (DE),A
  CALL     uptdir + 1
  JP       NZ,err4
  LD       HL,#0
  xor      a
label17:
  LD       SP,#0
  or       a
  ret

sub4:
  LD       A,#0xD4
  RST      0x28

sub:
  DEC      HL

label10:
  LD       A,(HL)
  INC      HL
  CP       #' '
  JR       Z,label10
  DEC      HL
  LD       B,#' '

label12:
  LD       A,(HL)
  CP       #'a'
  JR       C,label11
  SUB      #' '

label11:
  LD       (DE),A
  CP       #0x0D
  RET      Z
  CP       #'('
  RET      Z
  INC      HL
  INC      DE
  DJNZ     label12
  RET

sub2:
  PUSH     DE
  LD       A,(DE)
  CP       #'A'
  CALL     C,sub5
  LD       B,#'/'
  CALL     sub6
  LD       B,#'.'
  CALL     sub6
  POP      DE
  RET

label4:
  INC      DE

sub6:
  LD       A,(DE)
  CP       B
  JR       Z,label13
  CP       #'A'
  JR       NC,label4
  CP       #'0'
  JR       C,label5
  CP       #':'
  JR       C,label4
  JR       label5

label13:
  INC      DE
  RET

label5:
  PUSH     HL

label2:
  LD       A,(HL)
  INC      HL
  CP       #3
  JR       Z,label3
  CP       #0x0D
  JR       Z,label3
  CP       B
  JR       NZ,label2
  CALL     sub7

label3:
  POP      HL
  RET

sub5:
  LD       A,(HL)
  CP       #'0'
  RET      C
  CP       #':'
  JR       C,label1
  CP       #'A'
  RET      C

label1:
  INC      HL

sub7:
  PUSH     HL
  LD       H,D
  LD       L,E

label15:
  LD       C,(HL)
  LD       (HL),A
  INC      HL
  LD       A,C
  CP       #3
  JR       Z,label14
  CP       #0x0D
  JR       NZ,label15

label14:
  LD       (HL),A
  POP      HL
  INC      DE
  JR       sub5

label8:
  LD       A,#' '

err4:
  ld       a,#44        ; Parameter error
  JP       label17

err:
  ld       a,#19        ; Illegal file name
  jP       label17

err2:
  ld       a,#44        ; Parameter error
  jP       label17

uptdir:
  or       #0xaf   ; $AF = OP code for XOR A, Sets Z flag
  push     bc
  push     de
  push     af
  call     _dircyl
  ld       e,#1
  ld       hl,#0x4300
  pop      af
  jr       z,upd1
  call     _rdssec
  ld       a,#0x16
  jr       upd2
upd1:
  call     _wrprot
  call     z,_versec
  cp       #6
  ld       a,#0x17
upd2:
  pop      de
  pop      bc
  ret

fcb_from: .ds 32  ; 53C4H
fcb_to:   .ds 32  ; 53A4H

