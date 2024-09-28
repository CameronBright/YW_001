#ifndef __MAIN_H
#define __MAIN_H

//#define MAIN_Fosc		22118400L	//定义主时钟
//#define MAIN_Fosc		12000000L	//定义主时钟
 #define MAIN_Fosc		11059200L	//定义主时钟
//#define MAIN_Fosc		 5529600L	//定义主时钟
//#define MAIN_Fosc		24000000L	//定义主时钟

#include 	<intrins.h>
#include	"STC15Fxxxx.H"

sbit Buzzer = P2^6; //蜂鸣器 低电平工作
//sbit Buzzer = P3^7; //蜂鸣器太吵了先给他掐了

sbit CHLED = P3^4; //灯圈 低电平点亮

sbit LED1 = P4^7;  //LED1
sbit LED2 = P4^6;  //LED2
sbit LED3 = P1^2;  //LED3
sbit LED4 = P1^3;  //LED4

sbit HotWater = P1^0; //热水开关 低电平开水
sbit NtWater = P0^6;  //常温水开关 低电平开水
sbit Valve = P0^5;    //电磁阀开关 高电平打开
sbit Flow  = P0^4;    //流量计
sbit Mode  = P1^4;    //高原模式和普通模式

sbit PWM = P1^1;      //电机控制

#define ON 		1
#define OFF 	0
#define HOT 	0
#define COLD  1

#define CMD_IDLE    0               //????
#define CMD_READ    1               //IAP?????
#define CMD_PROGRAM 2               //IAP??????
#define CMD_ERASE   3               //IAP??????

sfr IAP_TPS = 0xF5;
#define IAP_ADDRESS 0x0400

#define P0INTE (*(unsigned char volatile xdata *)0xfd00)
#define P0INTF (*(unsigned char volatile xdata *)0xfd10)
#define P0IM0 (*(unsigned char volatile xdata *)0xfd20)
#define P0IM1 (*(unsigned char volatile xdata *)0xfd30)

#endif
