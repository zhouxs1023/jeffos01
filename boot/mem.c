#include "mem.h"
#include "conio.h"
#include "types.h"

unsigned long nMemMax;

unsigned int kmemcount();

void kinit_page(unsigned long loc) {
	int i;
	uint32 *flat;		// Points to page directory
	uint32 _cr3;		// Temp variable for cr3
	uint32 _cr0 = 0;	// Temp variable for cr0

	// Grab the last three pages of memory for page table use
	flat = (void*) ((4096*(nMemMax/4096)) - 4096*3);

	// Clear out the page tables and directories
	for(i = 0; i < 1024; i++)
	{
		// Clear out the page directory
		flat[i] = 0;

		// Map in the first 4MB as one to one addresses into low PageTbl
		flat[1024 + i] = 4096 * i | 3;

		// Map in 10 pages into high memory which are the kernel 
		flat[2048 + i] = i > 10 ? 0 : ((uint32)0x70000 + 4096 * i) | 3; 
	}

	// Set the Page Directory to have entries for the two page tables
	flat[0] = (uint32) &flat[1024] | 3;
	flat[512] = (uint32) &flat[2048] | 3;
	
	// Map in the page tables at 0x803FD000
	flat[2048 + 1021] = ((uint32) flat) | 3;
	flat[2048 + 1022] = ((uint32) flat + 4096) | 3;
	flat[2048 + 1023] = ((uint32) flat + 2*4096) | 3;
	

	// Set cr3 register to point to the page directory
	_cr3 = (uint32) flat;
	__asm__ ("mov %0, %%cr3"::"r" (_cr3));

	// Enable paging
	asm("mov %%cr0, %0":"=r" (_cr0));
	_cr0 = _cr0 | 0x80000000;
	asm("mov %0, %%cr0"::"r" (_cr0));

	return;
}


unsigned int kinit_MemMgmt(unsigned long loc) {
	unsigned int val;
	int i;

	// Start off believing that there is at least 1MB of memory
	nMemMax = 0x000fffff;

	// Count the amount of memory on the system
	val = kmemcount();

	// Set up the kernel page table
	kinit_page(loc);

	// Return the amount of memory on the system.
	return val;
}

unsigned int kmemcount() {
	char *Memory = (char *)0;
	unsigned int index = 0x1FFFFC;
	unsigned int memcount =  0;

	// Do a read/write/read test on each MB of memory

	// BUG - This is not working properly yet... Neesd to be fixed

	while(index < 0x3FFFFC) {
		Memory[index] = 17;
		if(Memory[index] != 17)
			goto Done;
		nMemMax = index + 3;
		memcount += 0x100000;
		index += 0x100000;
	}
Done:
	return memcount;
}

