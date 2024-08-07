
FORM_OBJS = \
	crt0.rel \
	main.rel \
	about.rel \
	demo-trs-nic.rel \
	demo-trs-fs.rel \
	demo-form.rel

IMPORT_OBJS = \
	crt0.rel \
	import.rel

TRS_LIB = ../lib/trs-lib.lib

CC = sdcc
AS = sdasz80
LD = sdldz80
IHEX2CMD = ../ihex2cmd/ihex2cmd
CFLAGS = -mz80 --std-sdcc11 --opt-code-size -I../lib/include
ASFLAGS = -l
LDFLAGS = -mjwx -b _CODE=0x5200 -l $(TRS_LIB) -l $(SDCC_PATH)/lib/z80/z80.lib

%.rel: %.c
	$(CC) $(CFLAGS) -c $< 

%.rel: %.s
	$(AS) $(ASFLAGS) -o $@ $< 

%.cmd: %.ihx
	$(IHEX2CMD) $< $@

all: demo.cmd import.cmd

demo.ihx: $(FORM_OBJS) $(TRS_LIB) Makefile
	$(LD) $(LDFLAGS) -i demo.ihx $(FORM_OBJS)

import.ihx: $(IMPORT_OBJS) $(TRS_LIB) Makefile
	$(LD) $(LDFLAGS) -i import.ihx $(IMPORT_OBJS)


clean:
	rm -f *.rel
	rm -f *.lk
	rm -f *.lst
	rm -f *~
	rm -f *.noi
	rm -f *.ihx
	rm -f *.map
	rm -f *.asm
	rm -f *.sym
	rm -f *.cmd

