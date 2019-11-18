/* $Id: ktrace.c,v 1.2 1997/10/03 11:52:40 swetland Exp $ */

/* serial tracing */

#include <stdarg.h>

#include "io.h"
#include "conio.h"

#define com1 0x3f8 
#define com2 0x2f8

#define combase com1

void va_snprintf(char *b, int l, char *fmt, va_list pvar);
static char kpbuf[256];

void kprintf(char *fmt, ...)
{
    va_list pvar;    
    va_start(pvar,fmt);
    va_snprintf(kpbuf,256,fmt,pvar);
    va_end(pvar);
    con_puts(kpbuf);    
}

#ifdef foo

void init_serial(void)
{
    outb(inb(combase + 3) | 0x80, combase + 3);
    outb(12, combase);                           /* 9600 bps, 8-N-1 */
    outb(0, combase+1);
    outb(inb(combase + 3) & 0x7f, combase + 3);
}

int getDebugChar(void)
{
    while (!(inb(combase + 5) & 0x01));
    return inb(combase);
}

void putDebugChar(int ch)
{
    while (!(inb(combase + 5) & 0x20));
    outb((char) ch, combase);
}

#endif
