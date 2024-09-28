#ifndef __MAIN_H
#define __MAIN_H

//#define MAIN_Fosc		22118400L	//定义主时钟
//#define MAIN_Fosc		12000000L	//定义主时钟
 #define MAIN_Fosc		11059200L	//定义主时钟
//#define MAIN_Fosc		 5529600L	//定义主时钟
//#define MAIN_Fosc		24000000L	//定义主时钟

#include 	<intrins.h>
#include	"STC15Fxxxx.H"

sbit Buzzer = P5^2; //蜂鸣器 低电平工作
//sbit Buzzer = P3^7; //蜂鸣器太吵了先给他掐了

sbit LED1 = P5^3;  //LED1
sbit LED2 = P0^5;  //LED2
sbit LED3 = P0^6;  //LED3
sbit LED4 = P1^3;  //LED4

#define ON 		1
#define OFF 	0

#endif
