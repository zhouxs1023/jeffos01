This directory contains the assembly code for the first stage floppy boot
loader.  In order to asseble it, you must obtain the a386 assembler noted
in intro.txt in the article directory or modify the code to assemble under
another assembler.

If you are using A386 simply type 'a386 first.asm'. This will result in a file
called first.bin which will contain the assembled binary. As A386 is a DOS 
based program this work will need to be done in DOS or Windows. These files will
need to be brought over to the linux or other box that you are using for
compilation of the kernel.

Running build.bat will copy the first.bin file into the build directory where
you can create a disk image to be written to a floppy disk.

Running clean.bat will remove all extraneous files from the directory except
for the first.asm file and the batch files.
