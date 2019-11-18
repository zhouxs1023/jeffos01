/* $Id: conio.h,v 1.2 1997/10/08 12:27:13 swetland Exp $ */

#ifndef _CONIO_H
#define _CONIO_H

#define CON_SCREEN	0x000B8000

#define CON_BLACK       0
#define CON_BLUE        1
#define CON_GREEN       2
#define CON_CYAN        3
#define CON_RED         4
#define CON_MAGENTA     5
#define CON_YELLOW      6
#define CON_WHITE       7

void kprintf(char *fmt,  ...);
void con_attr(int a);
void con_clear(void);
void con_putc(char ch);
void con_puts(char *s);
void con_putp(unsigned int p);
void con_putx(unsigned char x);

#define con_fgbg(fg,bg) con_attr((bg) << 4 | (fg));

#endif /* _CONIO_H */
