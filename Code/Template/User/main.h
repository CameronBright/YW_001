#ifndef __MAIN_H
#define __MAIN_H

//#define MAIN_Fosc		22118400L	//������ʱ��
//#define MAIN_Fosc		12000000L	//������ʱ��
 #define MAIN_Fosc		11059200L	//������ʱ��
//#define MAIN_Fosc		 5529600L	//������ʱ��
//#define MAIN_Fosc		24000000L	//������ʱ��

#include 	<intrins.h>
#include	"STC15Fxxxx.H"

sbit Buzzer = P5^2; //������ �͵�ƽ����
//sbit Buzzer = P3^7; //������̫�����ȸ�������

sbit LED1 = P5^3;  //LED1
sbit LED2 = P0^5;  //LED2
sbit LED3 = P0^6;  //LED3
sbit LED4 = P1^3;  //LED4

#define ON 		1
#define OFF 	0

#endif
