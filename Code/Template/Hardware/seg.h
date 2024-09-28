#ifndef __SEG_H
#define __SEG_H

#include "main.h"

sbit ADIG1 = P3^2;
sbit ADIG2 = P5^0;
sbit ADIG3 = P3^5;
sbit ADIG4 = P1^3;

sbit BDIG1 = P2^1;
sbit BDIG2 = P2^4;
sbit BDIG3 = P2^5;
sbit BDIG4 = P2^0;

sbit LED1A = P3^3;
sbit LED1B = P3^6;
sbit LED1C = P1^6;
sbit LED1D = P1^4;
sbit LED1E = P4^7;
sbit LED1F = P3^4;
sbit LED1G = P1^7;
sbit LED1DP = P1^5;

sbit LED2A = P2^2;
sbit LED2B = P2^6;
sbit LED2C = P4^3;
sbit LED2D = P4^1;
sbit LED2E = P3^7;
sbit LED2F = P2^3;
sbit LED2G = P4^4;
sbit LED2DP = P4^2;

void Led_Trans(unsigned char *seg_string,unsigned char *seg_buf);
void Seg_Disp1(unsigned char *seg_buf, unsigned char pos);
 
#endif
