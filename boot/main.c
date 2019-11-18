#include "conio.h"

// Create pointer to entry point in main kernel code
void (*k_start)(void) = (void (*)(void)) 0x80000080;

void _start(unsigned long blh) {
	unsigned int tmp;

	// Set up initial page tables in memory
	tmp = kinit_MemMgmt(blh);

	// Call the _start() function in the main kernel
	k_start();
	
}
