#ifndef		__KEY_H
#define		__KEY_H

#include "main.h"

sbit key1 = P4^4;
sbit key2 = P1^5;
sbit key3 = P1^6;
sbit key4 = P1^7;

unsigned char Key_Scan(void);
void Faucet_Switch(bit temp, bit Wswitch);  

#endif
