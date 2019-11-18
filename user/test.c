#include "syscall.h"

void main(void)
{
	unsigned long i, j;
    
blah:
	os_console("User thread...");   
	for(i = 0; i < 20000000; i++)
		j = 0;
	goto blah;
}


