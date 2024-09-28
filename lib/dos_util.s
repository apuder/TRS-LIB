
    .module dos
    .optsdcc -mz80

     .area _CODE   
CPM         .equ     0
TRSDOS      .equ     1

  .globl  exit
  .globl  abort
  .globl  error
  .globl  __renam

;; Model I/III addresses
__fspec  .equ 0x441c
__init   .equ 0x4420
__open   .equ 0x4424
__close  .equ 0x4428
__read   .equ 0x4436
__write  .equ 0x4439
__ramdir .equ 0x4290
__remov  .equ 0x442c
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
__ramdir6 .equ 35
__remov6 .equ 57
__renam6 .equ 56
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
read:	call __read
	ret
write:	call __write
  ret
ramdir: call __ramdir
  ret
remov: call __remov
  ret
renam: call __renam
  ret
error:	call __error
	ret
exit:	call __exit
	ret
abort:	call __abort
	ret
;dsply:	call dsply5
;	ret
setern:	call __setern5
	ret
getern:	call __getern5
	ret
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
	ld a,#__ramdir6
	rst __svc
	ret
	ld a,#__remov6
	rst __svc
	ret
	ld a,#__renam6
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
  call __setern6
  ret
	call __getern6
	ret

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

;; EOF handling differs between TRS-80 DOSes:
;;  For TRSDOS 2.3 and LDOS, word (dcb+12) contains the number of
;;  256 byte records in the file, byte (dcb+8) contains the EOF
;;  offset in the last record (0=256).
;;  For NEWDOS/80 and TRSDOS 1.3, byte (dcb+8) and word (dcb+12) 
;;  form a 24 bit number containing the relative byte address of EOF.
;;  Thus (dcb+12) differs by one if the file length is not a
;;  multiple of 256 bytes.  DOSPLUS also uses this convention,
;;  and NEWDOS 2.1 probably does too (not checked).

; Input: IX: pointer to FCB
; Returns number of (partial or full) records in DE, destroys A
__getern5:
	ld e,12(ix)
	ld d,13(ix)
	ld a,(ernldos)         ; get ERN convention
	and a
	ret nz                  ; done if TRSDOS 2.3/LDOS convention
	ld a,8(ix)		; length multiple of 256 bytes?
	and a
	ret z                   ; done if so
	inc de			; no, # of records = last full record + 1
	ret	

; All Model 4 mode operating systems should be TRSDOS/LS-DOS 6.x compatible
__getern6:
	ld e,12(ix)
	ld d,13(ix)
	ret

; Set ending record number of file to current position
; Input: IX: pointer to FCB
; Destroys A
__setern5:
  push hl
	ld l,10(ix)		; current record number
  ld h,11(ix)
	ld a, (ernldos)         ; get ERN convention
	or a
	jr nz, noadj            ; go if TRSDOS 2.3/LDOS convention
adj:
  ld c,8(ix)
  or c			; length multiple of 256 bytes?
	jr z, noadj             ; go if so
	dec hl			; no, # of records - 1
noadj:
  ld 12(ix),l
  ld 13(ix),h
  pop hl
	ret	

; All Model 4 mode operating systems should be TRSDOS/LS-DOS 6.x compatible
__setern6:
  push hl
	ld l,10(ix)
  ld h,11(ix)
	ld 12(ix),l
  ld 13(ix),h
  pop hl
	ret

;--------------------------------------------------------------------
; @fspec
;--------------------------------------------------------------------
  .globl _dos_fspec
_dos_fspec:
  push hl
  call fspec
  pop hl
  ld a,#44       ; Error 44: Parameter error
  ret nz
  xor a
  ret

;--------------------------------------------------------------------
; @open
;--------------------------------------------------------------------
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
  pop hl
  inc sp
  jp (hl)

;--------------------------------------------------------------------
; @init
;--------------------------------------------------------------------
  .globl _dos_init
_dos_init:
  push ix
  ld ix,#0
  add ix,sp
  ld b,4(ix)
  ex de,hl
  call init
  ex de,hl
  pop ix
  pop hl
  inc sp
  jp (hl)

;--------------------------------------------------------------------
; getern
;--------------------------------------------------------------------
  .globl _dos_getern
_dos_getern:
  push ix
  push hl
  pop ix
  call getern
  pop ix
  ret

;--------------------------------------------------------------------
; setern
;--------------------------------------------------------------------
  .globl _dos_setern
_dos_setern:
  push ix
  push hl
  pop ix
  call setern
  pop ix
  ret

;--------------------------------------------------------------------
; @read
;--------------------------------------------------------------------
  .globl _dos_read
_dos_read:
  push ix
  ex de,hl
  call read
  ex de,hl
  pop ix
  ret

;--------------------------------------------------------------------
; @write
;--------------------------------------------------------------------
  .globl _dos_write
_dos_write:
  push ix
  ex de,hl
  call write
  ex de,hl
  pop ix
  ret

;--------------------------------------------------------------------
; @close
;--------------------------------------------------------------------
  .globl _dos_close
_dos_close:
  ex de,hl
  call close
  ex de,hl
  ret

;--------------------------------------------------------------------
; @ramdir
;--------------------------------------------------------------------
  .globl _dos_ramdir
_dos_ramdir:
  pop de
  ld c,a
  ld b,l
  pop hl
  push de
  call ramdir
  pop hl
  jp (hl)

;--------------------------------------------------------------------
; @remov
;--------------------------------------------------------------------
  .globl _dos_remove
_dos_remove:
  ex de,hl
  call remov
  ex de,hl
  ret

;--------------------------------------------------------------------
; @renam
;--------------------------------------------------------------------
  .globl _dos_rename
_dos_rename:
  ex de,hl
  call renam
  ex de,hl
  ret
