/*

Version: 1.0.2
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
#include "uart.h"
//#include "eeprom.h"
#include "config.h"

//-----------����ѡ����-----------


//����ִ�к���
void Key_Proc(void);   	   //Keystroke process function
void Disp_Proc(void);  	   //LCD Dsiplay process function

//�Զ��幦�ܺ��� 
void Delay(unsigned int delay); //��ʱ����ʱ �Ῠס��ǰ������������Ӱ����������
void Peripheral_Init(void);//�����ʼ������

//���ڹ��ܺ���
void init_Uart2();//����2��ʼ��
void Uart2Send(char dat);
void Uart2SendStr(char *puts);//��������

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
char seg_string[4];			//�������ʾ�ַ���
char seg_buf1[4];				//�����1��ʾ������
char seg_buf2[4];				//�����2��ʾ������

u8 LED_buf;						//������LED���� 0b0011 1111 1Ϊ�� 0Ϊ��
u8 pos = 0;  						//����ܶ�ѡ

/*������ر���*/
u8  TX2_Cnt;    //U2���ͼ���

u8  RX2_Cnt;    //U2���ռ���

bit B_TX2_Busy; //U2����æ��־

u8  RX2_Buffer[16]; //u2���ջ���

/*������ر���*/
u16 buzzer_tick;        //�������������

/*������Ʊ���*/
u8 mode;                //ģʽ�л�

void main(){
	//P_SW2 |= 0x80;  //��չ�Ĵ���(XFR)����ʹ��
	GPIO_Init();			//���ų�ʼ��
	Timer1_Init();		//��ʱ����ʼ��
	
	init_Uart2();
	
	EA = 1;         	//�����ж�
	Config_Init();  	//��ʼ������
	Peripheral_Init();//���躯����ʼ״̬
	
	Uart2SendStr("STC8H TEST!");
	while(1)
	{
		
		
		Key_Proc();
		Disp_Proc();
		LED1 = 0;
		LED2 = 0;
		LED3 = 0;		
		Uart2SendStr("STC8H TEST!");
		
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

	Seg_Disp1(seg_buf1,pos);
	Seg_Disp2(seg_buf2,pos);
	if(++pos > 3) pos = 0;
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
				if(++mode > 3) mode = 0;
				break;
			}
			case 2://����2
			{
				if(--mode == 0) mode = 0;
				break;
			}
			case 3://����3
			{
				Buzzer ^= 1;
				break;
			}
			case 4://����4
			{  
				mode++;
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
	
	sprintf(seg_string,"---%d",(unsigned int)mode); 
	Led_Trans(seg_string,seg_buf1); 

	sprintf(seg_string,"----");
	Led_Trans(seg_string,seg_buf2); 
	
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
	LED_buf = 0x30;
 
}


//================����2���ܺ���=======================
void init_Uart2()//������11.0592

{
	 S2CON=0x10;   //���������
	 T2L = 0xE8;    //���ö�ʱ��ʼֵ
	 T2H = 0xFF;    //���ö�ʱ��ʼֵ
	 AUXR|=0x04;   //����T2��ʱ��1T����ģʽ
	 AUXR|=0x10;   //����T2��ʱ��T2R=1

	 IE2|=0x01;//����ES=1
	 P_SW2 &= 0xFE;//����2p10p11
	 B_TX2_Busy = 0;//æ���
	 TX2_Cnt = 0;//���ͼ���
	 RX2_Cnt = 0;//���ռ���
}

void Uart2Send(char dat)//u2���͵��ַ�

{
      while(B_TX2_Busy);
      B_TX2_Busy=1;
      S2BUF=dat;
}

void Delay1ms(unsigned char x)    //@12.000MHz

{
   unsigned char i, j;
   i = 16;
   j = 147;
   while(x--)
   {
      do
      {
        while (--j);
      } while (--i);
   } 
}

void Uart2SendStr(char *puts)//U2�����ַ���
{
     while(*puts)
      {
        Uart2Send(*puts++);
      }

}	

void UART2_isr (void) interrupt 8//Uart2�����ж����

{


    if((S2CON & 1) != 0)

    {

        S2CON &= ~1;    //Clear Rx flag

        RX2_Buffer[RX2_Cnt++] = S2BUF;

        RX2_Cnt&=0x0f;

          SBUF=S2BUF;//����������2

    }

    if((S2CON & 2) != 0)

    {

        S2CON &= ~2;    //Clear Tx flag

        B_TX2_Busy = 0;

    }

}

