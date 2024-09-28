
CC = sdcc
AS = sdasz80
LIB = sdar
CFLAGS = -mz80 --std-sdcc11 --opt-code-size -Iinclude
ASFLAGS = -l

OBJS = \
	init.rel \
	dos.rel \
	dos_util.rel \
	rename.rel \
	screen.rel \
	window.rel \
	form.rel \
	header.rel \
	list.rel \
	menu.rel \
	font.rel \
	key.rel \
	trs-io.rel \
	trs-nic.rel \
	trs-fs.rel \
	panic.rel

%.rel: %.c
	$(CC) $(CFLAGS) -c $< 

%.rel: %.s
	$(AS) $(ASFLAGS) -o $@ $< 

all: trs-lib.lib

trs-lib.lib : $(OBJS) Makefile
	$(LIB) -rc trs-lib.lib $(OBJS)


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
	rm -f *.lib

