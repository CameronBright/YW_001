#include "config.h"

void Config_Init(void)
{
	Buzzer = 1 ; //蜂鸣器上电默认关闭
	CHLED = 1;   //灯圈上电默认关闭
	HotWater = 1;//热水开关上电默认关闭
	NtWater = 1; //冷水开关上电默认关闭 
	Valve = 0;   //电磁阀上电默认关闭
	
//	LED1 = 0;//LED灯上电常亮
//	LED2 = 0;
//	LED3 = 0;
//	LED4 = 0;
}

void EXTI_Init(void)
{
	P_SW2 |= 0x80;  //扩展寄存器XFR访问使能
	P0IM0 = 0x00; //下降沿中断
	P0IM1 = 0x00;
	P0INTE |= 0x10; //使能 P0^4口中断
	P_SW2 &= ~0x80;
}
