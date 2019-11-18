#include "syscall.h"

// This is the one and only system call currently.
// It calls interrupt 22 and passes in a pointer (eax) to a string
// which will be printed
void os_console(char *string)
{
    asm("int $0x22"::"eax" ((unsigned int) string)); 
}
