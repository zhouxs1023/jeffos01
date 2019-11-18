#include "types.h"
#include "i386.h"
#include "io.h"

void i386SetSegment(void *entry,
                    uint32 base, uint32 limit,
                    uint8 rights, uint8 gran)
{
    *((uint32 *) entry) = (limit & 0xFFFF) | ((base & 0xFFFF) << 16);   

    *((uint32 *) ( ((char *) entry) + 4 ) ) =
        ((base & 0x00FF0000) >> 16) | (base & 0xFF000000) |
        (rights << 8) | ((gran & 0xF0) << 16) |
        ((limit & 0x000F000) << 4);
}

void i386SetTaskGate(void *entry, uint16 selector, uint8 rights)
{
    *((uint32 *) entry) = selector << 16;
    *((uint32 *) ( ((char *) entry) + 4 ) ) = (0x05 | (rights & 0xF0)) << 8;  
}

/* thanks to paul swanson for these... */

void i386ltr(uint32 selector) 
{
    __asm__ __volatile__ ("ltr %0": :"r" (selector));
}

void i386lidt(uint32 base, uint32 limit) 
{
    uint32 i[2];

    i[0] = limit << 16;
    i[1] = (uint32) base;
    __asm__ __volatile__ ("lidt (%0)": :"p" (((char *) i)+2));
}

void i386lgdt(uint32 base, uint32 limit) 
{
    uint32 i[2];

    i[0] = limit << 16;
    i[1] = base;
    __asm__ __volatile__ ("lgdt (%0)": :"p" (((char *) i)+2));
}

uint32 *i386sgdt(uint32 *limit) 
{
    uint32 gdtptr[2];
    __asm__ __volatile__ ("sgdt (%0)": :"p" (((char *) gdtptr)+2));
    *limit = gdtptr[0] >> 16;
    return (uint32 *) gdtptr[1];
}

#define PORTA0 0x20
#define PORTB0 0x21
#define PORTA1 0xA0
#define PORTB1 0xA1

void remap_irqs(void)
{
    outb_p(0x11, PORTA0);
    outb_p(0x30, PORTB0);
    outb_p(0x04, PORTB0);
    outb_p(0x01, PORTB0);
    outb_p(0xff, PORTB0);                      
    
    outb_p(0x11, PORTA1);
    outb_p(0x38, PORTB1);
    outb_p(0x02, PORTB1);
    outb_p(0x01, PORTB1);
    outb_p(0xff, PORTB1);                       
}

void unmap_irqs(void)
{
    outb_p(0x11, PORTA0);
    outb_p(0x08, PORTB0);
    outb_p(0x04, PORTB0);
    outb_p(0x01, PORTB0);
    outb_p(0x00, PORTB0);               
    
    outb_p(0x11, PORTA1);
    outb_p(0x70, PORTB1);
    outb_p(0x02, PORTB1);
    outb_p(0x01, PORTB1);
    outb_p(0x00, PORTB1); 
}

void unmask_irq(int irq)
{
    if(irq < 8)
        outb((inb(PORTB0) & ~(1 << irq)), PORTB0);
    else
        outb((inb(PORTB1) & ~(1 << (irq-8))), PORTB1);
}

void mask_irq(int irq)
{
    if(irq < 8)
        outb((inb(PORTB0) | (1 << irq)), PORTB0);
    else
        outb((inb(PORTB1) | (1 << (irq-8))), PORTB1);
}
