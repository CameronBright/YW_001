#include "key.h"

unsigned char Key_Scan(void)
{
	u8 key_value = 0;
	
	if(key1 == 0) //ͯ��
		return 1;
	if(key2 == 0) //��ˮ
		return 2;
	if(key3 == 0) //����ˮ
		return 3;
	if(key4 == 0) //�Ż�
		return 4;
	
	return key_value;
}

/*
temp: 0-ѡ����ˮ
			1-ѡ����ˮ

Wswitch��  
			1-�򿪳�ˮ��
			0-�رճ�ˮ��

*/
void Faucet_Switch(bit temp, bit Wswitch)
{
	if(Wswitch == 1)//�򿪿���
	{
	//	Valve = 1;//�򿪵�ŷ�
		if(temp == 0) //����ѡ��
		{
			HotWater = 0;//����ˮ
			NtWater = 1; //�رճ���ˮ
		}	
		
		if(temp == 1)
		{
			HotWater = 1;//�ر���ˮ 
			NtWater = 0; //�򿪳���ˮ
		}
	}
	
	if(Wswitch == 0)//�رտ���
	{
	//	Valve = 0; //�رյ�ŷ�
		HotWater = 1; //�ر���ˮ
		NtWater = 1;	//�رճ���ˮ
	} 
		
}
