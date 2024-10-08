/*

Version: 1.0.1
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

void UART2_config(u8 brt);   // ѡ������, 2: ʹ��Timer2��������, ����ֵ: ��Ч.
void PrintString2(u8 *puts);

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
u8  TX2_Cnt;    //���ͼ���
u8  RX2_Cnt;    //���ռ���
bit B_TX2_Busy; //����æ��־

u8  xdata RX2_Buffer[UART2_BUF_LENGTH]; //���ջ���

/*������ر���*/
u16 buzzer_tick;        //�������������

/*������Ʊ���*/
u8 mode;                //ģʽ�л�

void main(){
	GPIO_Init();			//���ų�ʼ��
	Timer1_Init();		//��ʱ����ʼ��
	UART2_config(2);  // ѡ������, 2: ʹ��Timer2��������, ����ֵ: ��Ч.
	EA = 1;         	//�����ж�
	Config_Init();  	//��ʼ������
	Peripheral_Init();//���躯����ʼ״̬
	
	PrintString2("STC8H8K64U UART2 Test Programme!\r\n");  //UART2����һ���ַ���
	
	while(1)
	{
		        if((TX2_Cnt != RX2_Cnt) && (!B_TX2_Busy))   //�յ�����, ���Ϳ���
        {
            S2BUF = RX2_Buffer[TX2_Cnt];
            B_TX2_Busy = 1;
            if(++TX2_Cnt >= UART2_BUF_LENGTH)   TX2_Cnt = 0;
        }
		
		
		Key_Proc();
		Disp_Proc();
		LED1 = 0;
		LED2 = 0;
		LED3 = 0;		
		
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
	//sprintf(seg_string,"0001"); 
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

//========================================================================
// ����: void PrintString2(u8 *puts)
// ����: ����2�����ַ���������
// ����: puts:  �ַ���ָ��.
// ����: none.
// �汾: VER1.0
// ����: 2014-11-28
// ��ע: 
//========================================================================
void PrintString2(unsigned char *puts)
{
    for (; *puts != 0;  puts++)     //����ֹͣ��0����
    {
        S2BUF = *puts;
        B_TX2_Busy = 1;
        while(B_TX2_Busy);
    }
}

//========================================================================
// ����: SetTimer2Baudraye(u16 dat)
// ����: ����Timer2�������ʷ�������
// ����: dat: Timer2����װֵ.
// ����: none.
// �汾: VER1.0
// ����: 2014-11-28
// ��ע: 
//========================================================================
void SetTimer2Baudraye(unsigned int dat)  // ʹ��Timer2��������.
{
    AUXR &= ~(1<<4);    //Timer stop
    AUXR &= ~(1<<3);    //Timer2 set As Timer
    AUXR |=  (1<<2);    //Timer2 set as 1T mode
    T2H = dat / 256;
    T2L = dat % 256;
    IE2  &= ~(1<<2);    //��ֹ�ж�
    AUXR |=  (1<<4);    //Timer run enable
}

//========================================================================
// ����: void UART2_config(u8 brt)
// ����: UART2��ʼ��������
// ����: brt: ѡ������, 2: ʹ��Timer2��������, ����ֵ: ��Ч. 
// ����: none.
// �汾: VER1.0
// ����: 2014-11-28
// ��ע: 
//========================================================================
void UART2_config(unsigned char brt)    // ѡ������, 2: ʹ��Timer2��������, ����ֵ: ��Ч.
{
    /*********** �����ʹ̶�ʹ�ö�ʱ��2 *****************/
    if(brt == 2)
    {
        SetTimer2Baudraye(65536UL - (MAIN_Fosc / 4) / Baudrate2);

        S2CON &= ~(1<<7);   // 8λ����, 1λ��ʼλ, 1λֹͣλ, ��У��
        IE2   |= 1;         //�����ж�
        S2CON |= (1<<4);    //�������
        P_SW2 &= ~0x01; 
        P_SW2 |= 0;         //UART2 switch to: 0: P1.0 P1.1,  1: P4.6 P4.7

        B_TX2_Busy = 0;
        TX2_Cnt = 0;
        RX2_Cnt = 0;
    }
}

//========================================================================
// ����: void UART2_int (void) interrupt UART2_VECTOR
// ����: UART2�жϺ�����
// ����: nine.
// ����: none.
// �汾: VER1.0
// ����: 2014-11-28
// ��ע: 
//========================================================================
void UART2_int (void) interrupt 8
{
    if((S2CON & 1) != 0)
    {
        S2CON &= ~1;    //Clear Rx flag
        RX2_Buffer[RX2_Cnt] = S2BUF;
        if(++RX2_Cnt >= UART2_BUF_LENGTH)   RX2_Cnt = 0;
    }

    if((S2CON & 2) != 0)
    {
        S2CON &= ~2;    //Clear Tx flag
        B_TX2_Busy = 0;
    }
}
