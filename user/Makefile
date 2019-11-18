CFLAGS = $(CF) -I../lib

all: test.bin install

install:
	cp test.bin ../build

test.bin: ../lib/crt0.o test.o ../lib/syscall.o
	$(LD) -dN -Ttext 20080 -o test.bin ../lib/crt0.o test.o ../lib/syscall.o
	$(NM) -nC test.bin > test.map
	$(ST) test.bin

clean:
	rm -f *.o *~ os.* core *.core *.s *.map *.bin
