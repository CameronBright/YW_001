/*

Version: 0.0.0
Check out the Readme.txt for more details 
 
*/
 
//ϵͳ��
#include "main.h"
#include <stdio.h>

//�����
#include "gpio.h"
#include "timer.h"
#include "key.h"
#include "seg.h"
//#include "eeprom.h"
#include "config.h"

//-----------����ѡ����-----------


//����ִ�к���
void Key_Proc(void);   	   //Keystroke process function
void Disp_Proc(void);  	   //LCD Dsiplay process function

//�Զ��幦�ܺ��� 
void Delay(unsigned int delay); //��ʱ����ʱ �Ῠס��ǰ������������Ӱ����������
void Peripheral_Init(void);//�����ʼ������

//----------------������Ҫ�޸ĵı���-------------------------------------

//ˢ�¼���,����10msɨ��һ��,�����200msˢ��һ��
u16 key_slow_down = 0; //����ˢ�¼���
u16 disp_slow_down = 0; //��ʾˢ�¼���

/*������ر���*/
u16 key_tick; 		   //long key press count
bit key_sw = 1;
bit keylong = 1;

u8 key_value = 0;				//����ֵ
u8 key_down;		 				//�����½���
u8 key_up;   		 				//������
u8 key_old;  		 				//�ϴεİ���ֵ

u16 delay_tick;         //�����ʱ����

/*�������ʾ��ر���*/
char seg_string[2];			//�������ʾ�ַ���
char seg_buf[2];				//�������ʾ������

u8 seg_buf2;						//������LED���� 0b0011 1111 1Ϊ�� 0Ϊ��
u8 pos1 = 0;  						//����ܶ�ѡ
u8 pos2 = 0;  						//����ܶ�ѡ

/*������ر���*/
u16 buzzer_tick;        //�������������

void main(){
	GPIO_Init();			//���ų�ʼ��
	Timer1_Init();		//��ʱ����ʼ��
	EA = 1;         	//�����ж�
	Config_Init();  	//��ʼ������
	Peripheral_Init();//���躯����ʼ״̬
	
	while(1)
	{
		Key_Proc();
		Disp_Proc();
		LED1 = 0; 
	}
} 

//================�жϺ���=======================
void Timer1_Isr(void) interrupt 3   //1ms �ж�һ��
{
	//���жϺ�����ȫ�Ǹ�ֵ��û�����㣬ÿ���ж�ʱ�䲻�����
	if(++key_slow_down == 10) key_slow_down = 0;   //10msɨ��һ�ΰ���
	if(++disp_slow_down == 200) disp_slow_down = 0;//200msˢ��һ�������
	 
//--------------------1s��ʱ��---------------------------
	

//--------------------�������---------------------------
	
	//�������̰���ʱ
	if(key_tick > 0) key_tick--;  
		
	//�����ʱ
	if(delay_tick > 0) delay_tick--;//��ʱ���� ʹ�ûῨס��ǰ����������Ӱ����������
		
	//����ܶ�ѡˢ��
//	Seg_Disp(seg_buf,pwm_index,seg_buf2,pos);
	if(++pos1 > 3) pos1 = 0;
	if(++pos2 > 3) pos2 = 0;
}

//================�����߼�����=======================
void Key_Proc(void)   	   //Keystroke process function
{
	if(key_slow_down) return;   //10ms����һ�δ˺���
		key_slow_down = 1;
		
	key_value = Key_Scan(); //��ȡ��ֵ ǿ������
	key_down = key_value & (key_value ^ key_old); //�½���
	key_up = ~key_value & (key_value ^ key_old);  //������
	key_old = key_value;
	
	if(key_down)       //����3��
		key_tick = 3000;
	
	 //key_old��ֵ tick���¼���  long��������֤��������ֻ����һ��
	if(key_old && key_tick == 0 && keylong){
		keylong = 0;		
		if(key_old == 1){ 		//����ͯ����		
			//------����1��������--------
			
			//------����1��������--------				
		}
		if(key_old == 4){ //�����Ż���	
			//------����4��������--------

			//------����4��������--------		
		}
	}
	
	if(key_up) //keydown-keyup˳���ܸ���
		keylong = 1;
	
	if(key_tick && key_sw){
		switch(key_up)
		{
			case 1://����1
			{ 		
				LED1 ^=1;
				break;
			}
			case 2://����2
			{
				LED2 ^= 1;
				break;
			}
			case 3://����3
			{
				LED3 ^= 1;
				break;
			}
			case 4://����4
			{    					
				break;                             
			}                            				 
			default:                             
				break;
		}
	}
		
}

//================��ʾ�߼�����=======================
void Disp_Proc(void)  	   //LCD Dsiplay process function
{
	if(disp_slow_down) return;   //200ms����һ�δ˺���(200msˢ��һ����Ļ)
		disp_slow_down = 1;
	
	
//	sprintf(seg_string,"%02d",(unsigned int)save_flag); //����
	 	
//	sprintf(seg_string,"%2d",(unsigned int)Mode);//��ʾģʽ 1=��ԭģʽ����ˮ85�㣬0=��ͨģʽ����ˮ93��
	
//	Led_Trans(seg_string,seg_buf);
		
}

//================�Զ��庯��=======================

//��ʱ����ʱ �Ῠס��ǰ������������Ӱ����������
void Delay(unsigned int delay) 
{
	delay_tick = delay;
	while(delay_tick > 0);
}

/*
tick: ���������ʱ��ms
*/
void Buzzer_Ctrl(u16 tick)  
{
	buzzer_tick = tick;
}

void Peripheral_Init(void)//�����ʼ������
{
	Buzzer_Ctrl(200);//�ϵ����200msһ��
	seg_buf2 = 0x30;
 
}

