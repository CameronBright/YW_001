#ifndef __UART_H
#define __UART_H

#define UART2_BUF_LENGTH    64

void PrintString2(unsigned char *puts);
void SetTimer2Baudraye(unsigned int dat);  // ʹ��Timer2��������.
void UART2_config(unsigned char brt);    // ѡ������, 2: ʹ��Timer2��������, ����ֵ: ��Ч.


#endif
