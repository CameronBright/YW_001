#include "gpio.h"

void GPIO_Init(void)
{
  P0M0 = 0x6f; P0M1 = 0x00; 
	P1M0 = 0x03; P1M1 = 0x00; 
	P2M0 = 0xbe; P2M1 = 0x00; 
  P3M0 = 0x00; P3M1 = 0x00; 
  P4M0 = 0x0c; P4M1 = 0x00; 
}
