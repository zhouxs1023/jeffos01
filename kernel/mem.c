#include "mem.h"
#include "conio.h"
#include "types.h"


unsigned long nPagesFree;
unsigned long nMemMax;
unsigned char PMAT[2048];
uint32 *flat;

unsigned int kmemcount();

void kinit_page() {
	int i;

	/* Get a pointer to the Page tables */
	flat = 0x803FD000;

	// Zero out the physical memory table - Each bit corresponds to
	// 1 page of physical memory.  so if PMAT[0] == 2 then
	// pages # 0 and 1 are allocated while pages 2 - 7 are unallocated
	for(i = 0; i < 2048; i++) {
		PMAT[i] = 0;
	}

	// Need to mark all of the allocated OS pages
	// Set aside the first two pages
	kset_PageBit(0);
	kset_PageBit(1);

	/* Mark the system memory as in use */
	// This memory contains the video ram and other BUS device memory
	for(i = 160; i < 256; i++)
		kset_PageBit(i);

	/* Mark pages which contain the kernel */
	// 70000h+
	kset_PageBit(112);
	kset_PageBit(113);
	kset_PageBit(114);
	kset_PageBit(115);
	kset_PageBit(116);
	kset_PageBit(117);
	kset_PageBit(118);
	kset_PageBit(119);
	kset_PageBit(120);
	kset_PageBit(121);

	/* Mark the page tables as used */
	/* The physical address of the kernel page directory is 
	 * equal to the physical address of the low page table - 4096 (the size
	 * of the page table)  We cannot use the pointer to the page directory
	 * as it is a mapped address rather than a physical address.
	 * By dividing this number by 4096 we get the page number or Bit to 
	 * set.
	 */
	kset_PageBit( ((unsigned long)flat[0] - 4096) / 4096);
	kset_PageBit(( ((unsigned long)flat[0] - 4096) / 4096) + 1);
	kset_PageBit(( ((unsigned long)flat[0] - 4096) / 4096) + 2);
	
}


void
kinit_tmpuser(void) {
	// Set aside the physical memory at 20000h for the user process
	kset_PageBit(32);
	kset_PageBit(33);
	kset_PageBit(34);
	kset_PageBit(35);
	kset_PageBit(36);
	kset_PageBit(37);
	kset_PageBit(38);
	kset_PageBit(39);
	kset_PageBit(40);
	kset_PageBit(41);
	kset_PageBit(42);
	kset_PageBit(43);
	kset_PageBit(44);
	kset_PageBit(45);
	kset_PageBit(46);
	kset_PageBit(47);
}


void *
mtop(void * memp) {
	unsigned long i;
	void *ret;

	// Find out the physical memory address of a paged memory address
	i = (((uint32)memp) - 0x80000000) / 4096;
	i = i + 2048;
	ret = (void *) (flat[i] & 0xfffff000);

	kprintf("Mapped memory: %x is physical address: %x\n", memp, ret);

	return ret;

}


void *
kmem_allocpagesphys(unsigned long npages, unsigned long flags, uint32 *phys) {
	unsigned long i;
	unsigned long j;

	// Allocate npages of physical memory, map them into memory, and return
	// a pointer to the location of the physical memory as well

	// A simple and crude search for 'npages' of contigous free physical 
	// memory
	for(i = 0; i < (16384 - npages); i++) {
		for(j = 0; j < npages; j++) {
			if(kget_PageBit(i + j)) {
				i += j;
				break;
			}
		}
		if(j == npages) {
			for(j = 0; j < npages; j++)
				kset_PageBit(i + j);
			i = i * 4096;
			goto mapmemory;
		}
	}
	kprintf("kmem: ERROR, there is not enough memory left to allocate %d pages\n", npages);
	return 0;

mapmemory:
	// i stores the beginning location of the physical memory to map in
	*phys = (uint32) i;

	// Debugging statement
	kprintf("allocated real memory of: 0x%x  0x%x pages\n", i, npages);

	// return the mapped in pointer of the memory
	return kmap_Page((void *)i, npages, flags);
}

			
void *
kmem_allocpages(unsigned long npages, unsigned long flags) {
	unsigned long i;
	unsigned long j;

	// Allocate npages of physical memory, and map them into memory

	// A simple and crude search for 'npages' of contigous free physical 
	// memory
	for(i = 0; i < (16384 - npages); i++) {
		for(j = 0; j < npages; j++) {
			if(kget_PageBit(i + j)) {
				i += j;
				break;
			}
		}
		if(j == npages) {
			for(j = 0; j < npages; j++)
				kset_PageBit(i + j);
			i = i * 4096;
			goto mapmemory;
		}
	}
	kprintf("kmem: ERROR, there is not enough memory left to allocate %d pages\n", npages);
	return 0;

mapmemory:
	// Debugging statement
	kprintf("allocated real memory of: 0x%x  0x%x pages\n", i, npages);

	// Return a pointer to the mapped in memory
	return kmap_Page((void *)i, npages, flags);
}
			

unsigned long
kmem_freepages(void * memp, unsigned long npages) {
	unsigned long i;
	unsigned long startbit;

	// Free npages of memory from the mapped in address starting at memp


	// Sanity checks
	i = (unsigned int)memp;
	if(i % 4096) {
		kprintf("kmem_free: Error trying to deallocate non 4k boundaried memory at: %x\n", memp);
		return -1;
	}
	if(!memp) {
		kprintf("kmem_free: Error trying to deallocate null pointer\n");
		return -1;
	}


	// First unmap the memory from the page tables
	startbit = (uint32) kunmap_Page(memp, npages);
	startbit = startbit / 4096;
	for(i = 0; i < npages; i++) {
		if(kget_PageBit(startbit + i))
			kunset_PageBit(startbit + i);
		else {
			kprintf("kmem_free: Trying to unset memory which is not set\n");
			return -1;
		}
	}
	return 0;
}

void *
kmap_Page(void *memp, unsigned long npages, unsigned long flags) {
	int i;
	int j;

	/* Map the physical memory pointer memp into the next free pages */
	for(i = 2048; i < (3072 - npages); i++) {
		for(j = 0; j < npages; j++) {
			if(flat[i + j]) {
				i += j;
				break;
			}
		}
		if(j == npages) {
			for(j = 0; j < npages; j++) {
				flat[i + j] = (((uint32)memp) + (4096 * j)) | flags;
			}
			goto done;
		}
	}
	kprintf("kmem_MapPage: ERROR, no space left in kernel memory to map physical memory into.\n");
	// Halt the kernel
	haltit();
	return (void *)0;

done:
	return (void *)(0x80000000 + (4096 * (i - 2048)));
}


void *
kunmap_Page(void *memp, unsigned long npages) {
	unsigned long i;
	unsigned long j;
	void *ret;

	i = (((uint32)memp) - 0x80000000) / 4096;
	ret = (void *) (flat[i] & 0xfffff000);

	for(j = 0; j < npages; j++) {
		if(!flat[i + j]) {
			kprintf("kunmap_Page: ERROR attempting to unmap page table which is already 0\n");
			return 0;
		}
		flat[i + j] = 0;
	}
	return ret;
}


unsigned int
kget_PageBit(unsigned long BitNum) {
	unsigned long Bit;
	unsigned long b = 0;

	b |= (1 << (BitNum % 8));
	Bit = PMAT[BitNum >> 3];
	Bit &= b;

	if(Bit)
		return 1;
	else
		return 0;
}

unsigned int 
kset_PageBit(unsigned int BitNum) {
        int b = 0;

        b |= (1 << (BitNum % 8));    // Find offset into byte to place bit
        PMAT[BitNum >> 3] |= b;       // AND in UnSet BIT
//kprintf("kset: BitNum: %x |=b: %x  PMAT[%d]: %x\n", BitNum, b, BitNum >> 3, PMAT[BitNum >> 3]);
}                          

unsigned int 
kunset_PageBit(unsigned int BitNum) {
        int b = 0;

        b |= (1 << (BitNum % 8));    // Find offset into byte to place bit
        PMAT[BitNum >> 3] &= ~b;       // AND in UnSet BIT
//kprintf("kunset: BitNum: %x &= b: %x  PMAT[%d]: %x\n", BitNum, ~b, BitNum >> 3,PMAT[BitNum >> 3]);
}                                   



unsigned int kmemcount() {
	char *Memory = (char *)0;
	unsigned int index = 0x1FFFFC;
	unsigned int memcount =  0;

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

void
memtest() {
	unsigned int tmp;
	void *pointer;
	void *pointer2;
	void *pointer3;
	void *pointer4;
	char *real;

	pointer = kmem_allocpages(3, 3);
	kprintf("alloced 3 pages at: %x\n", pointer);
	pointer2 = kmem_allocpages(5, 3);
	kprintf("alloced 5 pages at: %x\n", pointer2);


	mtop(pointer);
	pointer3 = mtop(pointer2);
	kprintf("real3 is: %x\n", pointer3);

real = (char *)pointer2;
real[25] = 25;
	tmp = kmem_freepages(pointer, 3);
	kprintf("Deallocated 3 pages with return of: %d\n", tmp);

	pointer3 = kmem_allocpages(5, 3);
	kprintf("alloced 5 pages at: %x\n", pointer3);
	pointer4 = kmem_allocpages(3, 3);
	kprintf("alloced 3 pages at: %x\n", pointer4);
	

	tmp = kmem_freepages(pointer2, 5);
	kprintf("Deallocated 5 pages with return of: %d\n", tmp);
	tmp = kmem_freepages(pointer3, 5);
	kprintf("Deallocated 5 pages with return of: %d\n", tmp);
	tmp = kmem_freepages(pointer4, 3);
	kprintf("Deallocated 3 pages with return of: %d\n", tmp);

}
