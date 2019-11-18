/* INT.H contains the header information for the interrupt handling routines */
#include "types.h"

#define SEL_KCODE (1*8)

typedef struct { uint32 edi, esi, ebp, esp, ebx, edx, ecx, eax; } regs;

static char *etable[] = {
    "Divide-by-zero",
    "Debug Exception",
    "NMI",
    "Breakpoint",
    "INTO",
    "BOUNDS",
    "Invalid Opcode",
    "Device Not Available",
    "Double-fault",
    "Coprocessor segment overrun",
    "Invalid TSS fault",
    "Segment Not Present",
    "Stack Exception",
    "General Protection",
    "Page Fault",
    "*reserved*",
    "Floating-point error",
    "Alignment Check",
    "Machine Check"
};           


extern void __null_irq(void);
extern void __enableirq(void);
extern void __timer_irq(void);
extern void __syscall22(void);
extern void __irq1(void);
extern void __irq2(void);
extern void __irq3(void);
extern void __irq4(void);
extern void __irq5(void);
extern void __irq6(void);
extern void __irq7(void);
extern void __irq8(void);
extern void __irq9(void);
extern void __irq10(void);
extern void __irq11(void);
extern void __irq12(void);
extern void __irq13(void);
extern void __irq14(void);
extern void __irq15(void);

void kadd_int(uint32 *IDT, unsigned long num, void *func);
void kadd_intU(uint32 *IDT, unsigned long num, void *func);

