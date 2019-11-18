#include "types.h"
#include "i386.h"
#include "int.h"
#include "conio.h"
#include "task.h"

int kernel_timer = 0;

extern task* CurrentTask;

uint32 *IDT;

#define EX(n) extern void __ex##n(void);
EX(0); EX(1); EX(2); EX(3); EX(4); EX(5); EX(6); EX(7); EX(8); EX(9);
EX(10); EX(11); EX(12); EX(13); EX(14); EX(15); EX(16); EX(17); EX(18);

kinit_idt() {
	unsigned long i;


	IDT = 0x00000000;
	for(i = 0; i < 256; i++)
		kadd_int(IDT, i, __null_irq);
	kadd_int(IDT, 0x30, __timer_irq);
	kadd_intU(IDT,0x22, __syscall22);

    kadd_int(IDT,0x00,__ex0);
    kadd_int(IDT,0x01,__ex1);
    kadd_int(IDT,0x02,__ex2);
    kadd_int(IDT,0x03,__ex3);
    kadd_int(IDT,0x04,__ex4);
    kadd_int(IDT,0x05,__ex5);
    kadd_int(IDT,0x06,__ex6);
    kadd_int(IDT,0x07,__ex7);
    kadd_int(IDT,0x08,__ex8);
    kadd_int(IDT,0x09,__ex9);
    kadd_int(IDT,0x0A,__ex10);
    kadd_int(IDT,0x0B,__ex11);
    kadd_int(IDT,0x0C,__ex12);
    kadd_int(IDT,0x0D,__ex13);
    kadd_int(IDT,0x0E,__ex14);
    kadd_int(IDT,0x0F,__ex15);

	kadd_int(IDT, 0x31, __irq1);
	kadd_int(IDT, 0x32, __irq2);
	kadd_int(IDT, 0x33, __irq3);
	kadd_int(IDT, 0x34, __irq4);
	kadd_int(IDT, 0x35, __irq5);
	kadd_int(IDT, 0x36, __irq6);
	kadd_int(IDT, 0x37, __irq7);
	kadd_int(IDT, 0x38, __irq8);
	kadd_int(IDT, 0x39, __irq9);
	kadd_int(IDT, 0x3a, __irq10);
	kadd_int(IDT, 0x3b, __irq11);
	kadd_int(IDT, 0x3c, __irq12);
	kadd_int(IDT, 0x3d, __irq13);
	kadd_int(IDT, 0x3e, __irq14);
	kadd_int(IDT, 0x3f, __irq15);


	i386lidt((uint32) IDT, 0x3FF);
	remap_irqs();
	unmask_irq(0);
	__enableirq();
}

void
kadd_int(uint32 *IDT, unsigned long num, void *func) {
	IDT[2*num+1] = (((uint32) func) & 0xFFFF0000) | 0x00008E00;
	IDT[2*num] = (((uint32) func) & 0x0000FFFF) | (SEL_KCODE << 16);
}

void
kadd_intU(uint32 *IDT, unsigned long num, void *func) {
	IDT[2*num+1] = (((uint32) func) & 0xFFFF0000) | 0x00008E00 | 0x00006000;
	IDT[2*num] = (((uint32) func) & 0x0000FFFF) | (SEL_KCODE << 16);
}

static void
dump_mem(uint32 cs, uint32 eip, uint32 len) {
	char *buf = 0;
	uint32 i;
	uint32 j;

	for(i = eip; i < (eip + len); i += 16) {
		kprintf("%x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x\n",
			buf[i] & 0xff, buf[i + 1] & 0xff, buf[i + 2] & 0xff, buf[i + 3] & 0xff,
			buf[i + 4] & 0xff, buf[i + 5] & 0xff, buf[i + 6] & 0xff, buf[i + 7] & 0xff,
			buf[i + 8] & 0xff, buf[i + 9] & 0xff, buf[i + 10] & 0xff, buf[i + 11] & 0xff,
			buf[i + 12] & 0xff, buf[i + 13] & 0xff, buf[i + 14] & 0xff, buf[i + 15] & 0xff);
	}

}

static void
print_regs(regs *r, uint32 eip, uint32 cs, uint32 eflags) {
	uint16 ds, es, gs, fs;

    kprintf("   EAX = %x   EBX = %x   ECX = %x   EDX = %x\n",
            r->eax, r->ebx, r->ecx, r->edx);
    kprintf("   EBP = %x   ESP = %x   ESI = %x   EDI = %x\n",
            r->ebp, r->esp, r->esi, r->edi);
    kprintf("EFLAGS = %x    CS = %x   EIP = %x\n",
            eflags, cs, eip);

	ds = es = fs = gs = 0;
	asm("mov %%ds, %%ax" : "=a" (ds));
	asm("mov %%es, %%ax" : "=a" (es));
	asm("mov %%fs, %%ax" : "=a" (fs));
	asm("mov %%gs, %%ax" : "=a" (gs));
	kprintf("DS = %x    ES = %x    GS = %x    FS = %x\n",
		ds, es, gs, fs);
	kprintf("KTSS DS = %x    ES = %x    GS = %x    FS = %x\n",
		CurrentTask->tss.ds, CurrentTask->tss.es,
		CurrentTask->tss.gs, CurrentTask->tss.fs);
}         

void 
dummy(regs r, uint32 eip, uint32 cs, uint32 eflags)
{
	kprintf("got an interrupt");
}


void 
timer_irq(void)
{
    kernel_timer++;
//kprintf("Timer IRQ called\n");
    swtch();

//if(kernel_timer == 2)
//	haltit();
}                     

void
syscall22(regs r, uint32 eip, uint32 cs, uint32 eflags) {

	//con_attr(CON_WHITE);
	con_fgbg(CON_WHITE, CON_BLUE);
	con_puts("\n");
	con_puts((char *)r.eax);
	con_fgbg(CON_WHITE, CON_BLUE);
	con_puts("\n");
}

void
faultE(uint32 number,
	regs r, uint32 error,
	uint32 eip, uint32 cs, uint32 eflags) {

	uint32 _cr2;
	con_attr(CON_RED);
	kprintf("\n*** Exception 0x%X* (%s)\n", number, etable[number]);
	print_regs(&r, eip, cs, eflags);
	dump_mem(cs, eip, 64);
halter:
	goto halter;
}

void fault(uint32 number,
           regs r,
           uint32 eip, uint32 cs, uint32 eflags)
{
    con_attr(CON_RED);
    kprintf("\n*** Exception 0x%X (%s)\n",number,etable[number]);
    print_regs(&r, eip, cs, eflags);
	dump_mem(cs, eip, 64);

halter:
	goto halter;

} 
