# NOTE: use gmake for this Makefile

#PRE = /develop/xdev/bin/
GCC = $(PRE)gcc
G++ = $(PRE)g++
LD = $(PRE)ld

OBJS = main.o conio.o ktrace.o snprintf.o mem.o
KERNELFN = BOOT.BIN

$(KERNELFN): $(OBJS)
	$(LD) -o $@ $(OBJS) -dN -Ttext 0x10080 
	cp BOOT.BIN ../build/

.cc.o: 
	$(G++) -O -I. -c -o $@ $< 

.cc.s:
	$(G++) -O -I. -S -o $@ $<

.c.o: 
	$(GCC) -O -I. -c -o $@ $< 

.c.s:
	$(GCC) -O -I. -S -o $@ $<

.S.o:
	$(GCC) -c -o $@ $<

clean: 
	rm $(OBJS) $(KERNELFN)
