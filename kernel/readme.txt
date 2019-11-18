This directory contains the source files for the kernel.

In order to compile the source you will need to have an ELF based compiler
which compiles to the x86 instruction set.  Simply type make and the KERNEL.BIN
file will be generated. To place the binary in /build/ do a make install.

The files in this directory are:

Makefile             Makefile for the kernel source
conio.c              console io functions
conio.h              console io header file
io.h                 The linux io.h file (useful for port io)
int.c                interrupt handler functions
int.h                interrupt handler function headers
jump.S               interrupt jump functions
test.c               test fuctions (unused)
taskt.c              task management functions
task.h               header file for task management functions
ktrace.c             kernel tracing functions
i386.c               386 specific functions (loading gdt, tasks etc..)
i386.h               header file for 386 instructions
main.c               main kernel routines ( _start() )
mem.c                memory management routines
mem.h                memory management header file
snprintf.h           printf routines
types.h              standard types definitions
