
    .module dos
    .optsdcc -mz80

     .area _CODE   
CPM         .equ     0
TRSDOS      .equ     1

;; Model I/III addresses
__fspec  .equ 0x441c
__init   .equ 0x4420
__open   .equ 0x4424
__close  .equ 0x4428
__read   .equ 0x4436
__write  .equ 0x4439
__error  .equ 0x4409
__exit   .equ 0x402d
__abort  .equ 0x4030       
__put	  .equ 0x001b
dodcb$	.equ 0x401d

;; Model 4 SVCs
__svc	  .equ 40  ; rst address for SVCs
__fspec6	.equ 78
__init6	.equ 58
__open6	.equ 59
__close6	.equ 60
__read6	.equ 67
__write6	.equ 75
__error6	.equ 26
__exit6	.equ 22
__abort6	.equ 21
__dsply6	.equ 10

;; Model 4 only: file init or open with wrong LRL.  Can be ignored.
lrlerr  .equ 42


;; Jump tables for OS independence
startj:
fspec:	call __fspec
	ret
init:	call __init
	ret
open:	call __open
	ret
close:	call __close
	ret
reed:	call __read
	ret
write:	call __write
	ret
error:	call __error
	ret
exit:	call __exit
	ret
abort:	call __abort
	ret
;dsply:	call dsply5
;	ret
;setern:	call setern5
;	ret
endj:

; Model 4
startj6:
	ld a,#__fspec6
	rst __svc
	ret
	ld a,#__init6
	rst __svc
	ret
	ld a,#__open6
	rst __svc
	ret
	ld a,#__close6
	rst __svc
	ret
	ld a,#__read6
	rst __svc
	ret
	ld a,#__write6
	rst __svc
	ret
	ld a,#__error6
	rst __svc
	ret
	ld a,#__exit6
	rst __svc
	ret
	ld a,#__abort6
	rst __svc
	ret
;	ld a,#__dsply6
;	rst __svc
;	ret
;	call setern6
;	ret

; Nonzero for LDOS ern convention
ernldos: .db 1

  .globl _init_dos
_init_dos: 
	ld a, (0x000a)		; Model 4?
	cp #0x40
	jr z, not4
	push hl
	ld de,#startj
	ld hl,#startj6
	ld bc,#endj - startj
	ldir
	pop hl
not4:
	ld a, (0x4427)		; system id for Newdos/80...
	sub #0x82			; ...should be 82h (v2.0)
	jr z, gotid
	ld a, (0x441f)		; system version number for most other DOSes
	sub #0x13			; TRSDOS 1.3?
gotid:  ld (ernldos), a
  ret

  .globl _dos_fspec
_dos_fspec:
  push hl
  call fspec
  pop hl
  ld a,#44       ; Error 44: Parameter error
  ret nz
  xor a
  ret

  .globl _dos_open
_dos_open:
  push ix
  ld ix,#0
  add ix,sp
  ld b,4(ix)
  ex de,hl
  call open
  ex de,hl
  pop ix
  ret