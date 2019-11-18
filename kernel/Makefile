# NOTE: use gmake for this Makefile

CFLAGS = $(CF) -I ./

all: kernel.bin install

install:
	cp kernel.bin ../build


OBJS = main.o conio.o ktrace.o snprintf.o mem.o jump.o int.o task.o i386.o

kernel.bin: $(OBJS)
	$(LD) -dN -Ttext 80000080 -o kernel.bin $(OBJS)
	$(NM) -nC kernel.bin > kernel.map
	$(ST) kernel.bin

jump.o: jump.S
	$(CC) -c jump.S

clean:
	rm -f *.o *~ os.* core *.core *.s *.map *.bin *.BIN
