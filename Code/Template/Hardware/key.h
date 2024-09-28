#ifndef		__KEY_H
#define		__KEY_H

#include "main.h"

sbit key1 = P0^1;
sbit key2 = P0^0;
sbit key3 = P4^6;
sbit key4 = P4^5;

unsigned char Key_Scan(void);

#endif
