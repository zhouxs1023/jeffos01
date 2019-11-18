#ifndef _I386_H
#define _I386_H

/* rights bits options */
#define i386rPRESENT 0x80   /* segment is present */

#define i386rDPL0    0x00
#define i386rDPL1    0x20
#define i386rDPL2    0x40
#define i386rDPL3    0x60

/* for Data/Code/TSS segments */
#define i386rDATA    0x12   /* segment is data, read/write */
#define i386rDATAro  0x10   /* segment is data, read only */
#define i386rCODE    0x1A   /* segment is code, read/exec */
#define i386rCODExo  0x18   /* segment is code, read only */

#define i386rTSS     0x09   /* segment is an i386 TSS */

/* gran bits options */
#define i386g4K      0x80   /* segment gran is 4K (else 1B) */
#define i386g32BIT   0x40   /* segment default is 32bit (else 16bit) */
#define i386gAVL     0x10   /* segment AVL is set */

void i386SetSegment(void *entry,
                    uint32 base, uint32 limit,
                    uint8 rights, uint8 gran);
void i386SetTaskGate(void *entry, uint16 selector, uint8 rights);

void i386ltr(uint32 selector);
void i386lidt(uint32 base, uint32 limit);
void i386lgdt(uint32 base, uint32 limit);
uint32 *i386sgdt(uint32 *limit);
void remap_irqs(void);

#define I386_PAGING_ON() asm("push %eax; mov %cr0, %eax; orl $0x80000000,%eax ; mov %eax, %cr0 ; pop %eax");
#define I386_PAGING_OFF() asm("push %eax; mov %cr0, %eax; andl $0x7FFFFFFF,%eax ; mov %eax, %cr0 ; pop %eax");

void unmap_irqs(void);
void remap_irqs(void);
void unmask_irq(int irq);
void mask_irq(int irq);

#endif _I386_H

