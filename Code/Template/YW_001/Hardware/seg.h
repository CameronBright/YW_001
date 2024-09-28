#ifndef __SEG_H
#define __SEG_H

#include "main.h"

sbit DIG1 = P2^5;
sbit DIG2 = P2^4;
sbit DIG3 = P2^7;

sbit COM1 = P4^2;
sbit COM2 = P0^3;

sbit LEDA = P2^1;
sbit LEDB = P2^2;
sbit LEDC = P2^3;
sbit LEDD = P0^0;
sbit LEDE = P0^1;
sbit LEDF = P0^2;
sbit LEDG = P4^3;

void Led_Trans(unsigned char *seg_string,unsigned char *seg_buf);
void Seg_Disp(unsigned char *seg_buf,unsigned char seg_buf1,unsigned char seg_buf2, unsigned char pos);
 
#endif
