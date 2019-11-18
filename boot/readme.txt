This directory contains the source files for the first stage boot loader.

In order to compile the source you will need to have an ELF based compiler
which compiles to the x86 instruction set. Simply type make and the BOOT.BIN
file will be generated and copied into \build\

The files in this directory are:

conio.c - contains console IO routines
conio.h - conio header file
ktrace.c - contains kernel tracing routinges
io.h - contains header information which really isn't used here.
main.c - contains the stub code to be jumped into
mem.c - contains memory initialization code
snprintf.c - contains a printf implementation
types.h contains definitions for some common types

