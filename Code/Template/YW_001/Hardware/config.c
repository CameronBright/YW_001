#include "config.h"

void Config_Init(void)
{
	Buzzer = 1 ; //�������ϵ�Ĭ�Ϲر�
	CHLED = 1;   //��Ȧ�ϵ�Ĭ�Ϲر�
	HotWater = 1;//��ˮ�����ϵ�Ĭ�Ϲر�
	NtWater = 1; //��ˮ�����ϵ�Ĭ�Ϲر� 
	Valve = 0;   //��ŷ��ϵ�Ĭ�Ϲر�
	
//	LED1 = 0;//LED���ϵ糣��
//	LED2 = 0;
//	LED3 = 0;
//	LED4 = 0;
}

void EXTI_Init(void)
{
	P_SW2 |= 0x80;  //��չ�Ĵ���XFR����ʹ��
	P0IM0 = 0x00; //�½����ж�
	P0IM1 = 0x00;
	P0INTE |= 0x10; //ʹ�� P0^4���ж�
	P_SW2 &= ~0x80;
}
