#include "key.h"

unsigned char Key_Scan(void)
{
	u8 key_value = 0;
	
	if(key1 == 0) //童锁
		return 1;
	if(key2 == 0) //开水
		return 2;
	if(key3 == 0) //常温水
		return 3;
	if(key4 == 0) //磁化
		return 4;
	
	return key_value;
}

/*
temp: 0-选择热水
			1-选择常温水

Wswitch：  
			1-打开出水口
			0-关闭出水口

*/
void Faucet_Switch(bit temp, bit Wswitch)
{
	if(Wswitch == 1)//打开开关
	{
	//	Valve = 1;//打开电磁阀
		if(temp == 0) //开关选择
		{
			HotWater = 0;//打开热水
			NtWater = 1; //关闭常温水
		}	
		
		if(temp == 1)
		{
			HotWater = 1;//关闭热水 
			NtWater = 0; //打开常温水
		}
	}
	
	if(Wswitch == 0)//关闭开关
	{
	//	Valve = 0; //关闭电磁阀
		HotWater = 1; //关闭热水
		NtWater = 1;	//关闭常温水
	} 
		
}
