#include "conio.h"

static int attr = 1, px = 0, py = 24;
static char *screen = (char *) CON_SCREEN;
static char *posn   = (char *) CON_SCREEN + 80*24*2;

static void scrollup(void) 
{
	for(posn = screen + 160; posn <= screen + 80*25*2; posn++)
		*(posn - 160) = *posn;
	for(posn = screen + 80*24*2; posn <= screen + 80*25*2; posn++){
		*posn++ = ' ';
		*posn   = attr;
	}
}

void con_attr(int a)
{
	attr = a;
}

void con_clear(void) 
{
	int i;

	for(posn = screen, i=0;i<80*25*2;i++){
		*posn++ = ' ';
		*posn++ = attr;
	}
	px = 0;
	py = 24;
	posn = screen + (24*80)*2;
}
/*
void con_putc(char ch)
{
	if(ch == '\n'){
		goto roll0;
	}
	*posn++ = ch;
	*posn++ = attr;
	px++;
	if(px == 80){
roll0:
		px = 0;
		if(py == 24)
			scrollup();
		else
			py++;
		posn = screen + (py*80+px)*2;
	}
}
*/
void con_puts(char *s)
{
	while(*s){
		if(*s == '\n'){
			s++;
			goto roll1;
		}
		*posn++ = *s++;
		*posn++ = attr;
		px++;
		if(px == 80){
roll1:
			px = 0;
			if(py == 24)
				scrollup();
			else
				py++;
			posn = screen + (py*80+px)*2;
		}
	}
}
/*
static char pbuf[9];

void con_putp(unsigned int p)
{
	int i;

	pbuf[8] = 0;
	
	for(i=7;i>=0;i--){
		pbuf[i] = (0x0F & p);
		if(pbuf[i] > 9) pbuf[i] += 'A'-10;
		else pbuf[i] += '0';
		p >>= 4;
	}
	con_puts(pbuf);
}

void con_putx(unsigned char x)
{
    pbuf[0] = (x & 0xF0) >> 4;
    if(pbuf[0] > 9) pbuf[0] += 'A'-10;
    else pbuf[0] += '0';
    pbuf[1] = (x & 0x0F);
    if(pbuf[1] > 9) pbuf[1] += 'A'-10;
    else pbuf[1] += '0';
    pbuf[2] = 0;
    con_puts(pbuf);    
}
*/
