/*

Version: 1.0.2
Check out the Readme.txt for more details 
 
*/
 
//系统库
#include "main.h"
#include <stdio.h>

//外设库
#include "gpio.h"
#include "timer.h"
#include "key.h"
#include "seg.h"
#include "uart.h"
//#include "eeprom.h"
#include "config.h"

//-----------功能选择区-----------


//程序执行函数
void Key_Proc(void);   	   //Keystroke process function
void Disp_Proc(void);  	   //LCD Dsiplay process function

//自定义功能函数 
void Delay(unsigned int delay); //定时器延时 会卡住当前函数，但不会影响整个代码
void Peripheral_Init(void);//外设初始化函数

//串口功能函数
void init_Uart2();//串口2初始化
void Uart2Send(char dat);
void Uart2SendStr(char *puts);//发送数据

//----------------可能需要修改的变量-------------------------------------

//刷新计数,按键10ms扫描一次,数码管200ms刷新一次
u16 key_slow_down = 0; //按键刷新计数
u16 disp_slow_down = 0; //显示刷新计数

/*按键相关变量*/
u16 key_tick; 		   //long key press count
bit key_sw = 1;
bit keylong = 1;

u8 key_value = 0;				//按键值
u8 key_down;		 				//按键下降沿
u8 key_up;   		 				//上升沿
u8 key_old;  		 				//上次的按键值

u16 delay_tick;         //软件延时计数

/*数码管显示相关变量*/
char seg_string[4];			//数码管显示字符串
char seg_buf1[4];				//数码管1显示缓冲区
char seg_buf2[4];				//数码管2显示缓冲区

u8 LED_buf;						//按键下LED控制 0b0011 1111 1为亮 0为灭
u8 pos = 0;  						//数码管段选

/*串口相关变量*/
u8  TX2_Cnt;    //U2发送计数

u8  RX2_Cnt;    //U2接收计数

bit B_TX2_Busy; //U2发送忙标志

u8  RX2_Buffer[16]; //u2接收缓冲

/*外设相关变量*/
u16 buzzer_tick;        //蜂鸣器短响计数

/*代码控制变量*/
u8 mode;                //模式切换

void main(){
	//P_SW2 |= 0x80;  //扩展寄存器(XFR)访问使能
	GPIO_Init();			//引脚初始化
	Timer1_Init();		//定时器初始化
	
	init_Uart2();
	
	EA = 1;         	//打开总中断
	Config_Init();  	//初始化配置
	Peripheral_Init();//外设函数初始状态
	
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

//================中断函数=======================
void Timer1_Isr(void) interrupt 3   //1ms 中断一次
{
	//本中断函数内全是赋值，没有运算，每次中断时间不会过长
	if(++key_slow_down == 10) key_slow_down = 0;   //10ms扫描一次按键
	if(++disp_slow_down == 200) disp_slow_down = 0;//200ms刷新一次数码管
	 
//--------------------1s定时器---------------------------
	

//--------------------外设控制---------------------------
	
	//按键长短按计时
	if(key_tick > 0) key_tick--;  
		
	//软件延时
	if(delay_tick > 0) delay_tick--;//延时函数 使用会卡住当前函数，但不影响其他功能
		
	//数码管段选刷新

	Seg_Disp1(seg_buf1,pos);
	Seg_Disp2(seg_buf2,pos);
	if(++pos > 3) pos = 0;
}

//================按键逻辑函数=======================
void Key_Proc(void)   	   //Keystroke process function
{
	if(key_slow_down) return;   //10ms进入一次此函数
		key_slow_down = 1;
		
	key_value = Key_Scan(); //读取键值 强制消抖
	key_down = key_value & (key_value ^ key_old); //下降沿
	key_up = ~key_value & (key_value ^ key_old);  //上升沿
	key_old = key_value;
	
	if(key_down)       //长按3秒
		key_tick = 3000;
	
	 //key_old键值 tick按下计数  long自锁，保证按键代码只触发一次
	if(key_old && key_tick == 0 && keylong){
		keylong = 0;		
		if(key_old == 1){ 		//长按童锁键		
			//------按键1长按功能--------
			
			//------按键1长按功能--------				
		}
		if(key_old == 4){ //长按磁化键	
			//------按键4长按功能--------

			//------按键4长按功能--------		
		}
	}
	
	if(key_up) //keydown-keyup顺序不能更改
		keylong = 1;
	
	if(key_tick && key_sw){
		switch(key_up)
		{
			case 1://按键1
			{ 		
				if(++mode > 3) mode = 0;
				break;
			}
			case 2://按键2
			{
				if(--mode == 0) mode = 0;
				break;
			}
			case 3://按键3
			{
				Buzzer ^= 1;
				break;
			}
			case 4://按键4
			{  
				mode++;
				break;                             
			}                            				 
			default:                             
				break;
		}
	}
		
}

//================显示逻辑函数=======================
void Disp_Proc(void)  	   //LCD Dsiplay process function
{
	if(disp_slow_down) return;   //200ms进入一次此函数(200ms刷新一次屏幕)
		disp_slow_down = 1;
	
	sprintf(seg_string,"---%d",(unsigned int)mode); 
	Led_Trans(seg_string,seg_buf1); 

	sprintf(seg_string,"----");
	Led_Trans(seg_string,seg_buf2); 
	
}

//================自定义函数=======================

//定时器延时 会卡住当前函数，但不会影响整个代码
void Delay(unsigned int delay) 
{
	delay_tick = delay;
	while(delay_tick > 0);
}

/*
tick: 蜂鸣器响的时间ms
*/
void Buzzer_Ctrl(u16 tick)  
{
	buzzer_tick = tick;
}

void Peripheral_Init(void)//外设初始化函数
{
	Buzzer_Ctrl(200);//上电短响200ms一次
	LED_buf = 0x30;
 
}


//================串口2功能函数=======================
void init_Uart2()//波特率11.0592

{
	 S2CON=0x10;   //打开允许接收
	 T2L = 0xE8;    //设置定时初始值
	 T2H = 0xFF;    //设置定时初始值
	 AUXR|=0x04;   //开启T2定时器1T工作模式
	 AUXR|=0x10;   //开启T2定时器T2R=1

	 IE2|=0x01;//开启ES=1
	 P_SW2 &= 0xFE;//串口2p10p11
	 B_TX2_Busy = 0;//忙检测
	 TX2_Cnt = 0;//发送计数
	 RX2_Cnt = 0;//接收计数
}

void Uart2Send(char dat)//u2发送单字符

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

void Uart2SendStr(char *puts)//U2发送字符串
{
     while(*puts)
      {
        Uart2Send(*puts++);
      }

}	

void UART2_isr (void) interrupt 8//Uart2串口中断入口

{


    if((S2CON & 1) != 0)

    {

        S2CON &= ~1;    //Clear Rx flag

        RX2_Buffer[RX2_Cnt++] = S2BUF;

        RX2_Cnt&=0x0f;

          SBUF=S2BUF;//发送至串口2

    }

    if((S2CON & 2) != 0)

    {

        S2CON &= ~2;    //Clear Tx flag

        B_TX2_Busy = 0;

    }

}

