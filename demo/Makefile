
FORM_OBJS = \
	crt0.rel \
	main.rel \
	about.rel \
	demo-trs-nic.rel \
	demo-trs-fs.rel \
	demo-form.rel

HEXDUMP_OBJS = \
	crt0.rel \
	hexdump.rel

LS_OBJS = \
	crt0.rel \
	ls.rel

CP_OBJS = \
	crt0.rel \
	cp.rel

REMOVE_OBJS = \
  crt0.rel \
	remove.rel

MV_OBJS = \
  crt0.rel \
	mv.rel

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

all: demo.cmd hexdump.cmd ls.cmd cp.cmd remove.cmd mv.cmd

demo.ihx: $(FORM_OBJS) $(TRS_LIB) Makefile
	$(LD) $(LDFLAGS) -i demo.ihx $(FORM_OBJS)

hexdump.ihx: $(HEXDUMP_OBJS) $(TRS_LIB) Makefile
	$(LD) $(LDFLAGS) -i hexdump.ihx $(HEXDUMP_OBJS)

ls.ihx: $(LS_OBJS) $(TRS_LIB) Makefile
	$(LD) $(LDFLAGS) -i ls.ihx $(LS_OBJS)

cp.ihx: $(CP_OBJS) $(TRS_LIB) Makefile
	$(LD) $(LDFLAGS) -i cp.ihx $(CP_OBJS)

remove.ihx: $(REMOVE_OBJS) $(TRS_LIB) Makefile
	$(LD) $(LDFLAGS) -i remove.ihx $(REMOVE_OBJS)

mv.ihx: $(MV_OBJS) $(TRS_LIB) Makefile
	$(LD) $(LDFLAGS) -i mv.ihx $(MV_OBJS)


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

