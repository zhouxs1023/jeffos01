This directory contains the files to build the user level object files.

To compile this source simply type make using an ELF compiler with similar
requirements as for the kernel.

The files contained in this directory are:

Makefile                 Makes the source in this directory
crt0.c                   Contains stub code
syscall.c                Contains the system call function
syscall.h                Header file for the system call
