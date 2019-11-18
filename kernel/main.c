#include "types.h"
#include "i386.h"
#include "conio.h"
#include "mem.h"
#include "int.h"
#include "task.h"
#include "io.h"

extern unsigned long nMemMax;
extern void __testint(void);

void _start(unsigned int blh) {
	task *t;
	uint32 sp;
	uint32 i;
	uint32 j, k;
	char *jtest;

	// Set up the console
	con_fgbg(CON_WHITE, CON_BLUE);
	con_clear();
	kprintf("Main kernel loaded.\n");

	// Initialize the Memory Management Subsystem
	kinit_page();

	// Initialize a simple user space
	kinit_tmpuser();


	// Run some simple memory tests
//	memtest();

	// Initialize the kernel task
	kinit_task();

	/* WARNING: Do not allocate any memory before here.  kinit_task
	 *     relocates the gdt into mapped memory
	 */

	// Allocate a new stack for the kernel
	kprintf("allocating stack\n");
	i = (uint32) kmem_allocpages(1, 3);
	asm("mov %0, %%esp"::"r" ( ((uint32) i + 4092) ));
	kprintf("esp is: 0x%x\n", i);


	// Create a new task for the loaded in user process
	// The starting ip for this task is 20080h and its stack is placed
	// at the end at 2107Ch(linear)
	t = task_create(0x00020080, 0x0002107c);

	// The correct behavior is to map in some basic locations for
	// the task (such as video memory area).
	// Currently task_create is mapping everything in so we do not
	// have to do this right now.  We will later
#ifdef ndef
	/* Map in code */
	aspace_map(t->addr, 0x20, 0x20, 0x10 + 2, 7);
	/* Map in video ram */
	aspace_map(t->addr, 0xB0, 0xB0, 0x10 + 2, 7);
        // Clear out the page tables and directories
        for(i = 0; i < 1024; i++)
        {
                t->addr->ptab[i] = 4096 * i | 7;
        }
#endif

	// Add the user task to the task queue with a priority of 0
	task_add(t, 0);


	// Print out the first byte of the user program to test if it is
	// correct - This is purely for my own debugging it can be removed
	jtest = 0;
	kprintf("User Code: %x\n", jtest[0x20080]);


	// Initialize the Interrupt Descriptor Table and start up the
	// interrupt timer
	kinit_idt();


	// Switch to a new task
	swtch();

	// The kernel task just goes through these simple operations to
	// prove that it is still running
	kprintf("Going into null loop\n");

blah:
	// Print out to console to prove the kernel is still alive
	kprintf("\nKernel Thread...\n");
	// Loop a whole bunch to add some delay
	for(i = 0; i < 20000000; i++)
		j = 0;
	// Do it again
	goto blah;
	
}

