#ifndef __UART_H
#define __UART_H

#define UART2_BUF_LENGTH    64

void PrintString2(unsigned char *puts);
void SetTimer2Baudraye(unsigned int dat);  // 使用Timer2做波特率.
void UART2_config(unsigned char brt);    // 选择波特率, 2: 使用Timer2做波特率, 其它值: 无效.


#endif
