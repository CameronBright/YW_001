/*

Version: 1.0.1
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

void UART2_config(u8 brt);   // 选择波特率, 2: 使用Timer2做波特率, 其它值: 无效.
void PrintString2(u8 *puts);

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
u8  TX2_Cnt;    //发送计数
u8  RX2_Cnt;    //接收计数
bit B_TX2_Busy; //发送忙标志

u8  xdata RX2_Buffer[UART2_BUF_LENGTH]; //接收缓冲

/*外设相关变量*/
u16 buzzer_tick;        //蜂鸣器短响计数

/*代码控制变量*/
u8 mode;                //模式切换

void main(){
	GPIO_Init();			//引脚初始化
	Timer1_Init();		//定时器初始化
	UART2_config(2);  // 选择波特率, 2: 使用Timer2做波特率, 其它值: 无效.
	EA = 1;         	//打开总中断
	Config_Init();  	//初始化配置
	Peripheral_Init();//外设函数初始状态
	
	PrintString2("STC8H8K64U UART2 Test Programme!\r\n");  //UART2发送一个字符串
	
	while(1)
	{
		        if((TX2_Cnt != RX2_Cnt) && (!B_TX2_Busy))   //收到数据, 发送空闲
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
	//sprintf(seg_string,"0001"); 
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

//========================================================================
// 函数: void PrintString2(u8 *puts)
// 描述: 串口2发送字符串函数。
// 参数: puts:  字符串指针.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void PrintString2(unsigned char *puts)
{
    for (; *puts != 0;  puts++)     //遇到停止符0结束
    {
        S2BUF = *puts;
        B_TX2_Busy = 1;
        while(B_TX2_Busy);
    }
}

//========================================================================
// 函数: SetTimer2Baudraye(u16 dat)
// 描述: 设置Timer2做波特率发生器。
// 参数: dat: Timer2的重装值.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void SetTimer2Baudraye(unsigned int dat)  // 使用Timer2做波特率.
{
    AUXR &= ~(1<<4);    //Timer stop
    AUXR &= ~(1<<3);    //Timer2 set As Timer
    AUXR |=  (1<<2);    //Timer2 set as 1T mode
    T2H = dat / 256;
    T2L = dat % 256;
    IE2  &= ~(1<<2);    //禁止中断
    AUXR |=  (1<<4);    //Timer run enable
}

//========================================================================
// 函数: void UART2_config(u8 brt)
// 描述: UART2初始化函数。
// 参数: brt: 选择波特率, 2: 使用Timer2做波特率, 其它值: 无效. 
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void UART2_config(unsigned char brt)    // 选择波特率, 2: 使用Timer2做波特率, 其它值: 无效.
{
    /*********** 波特率固定使用定时器2 *****************/
    if(brt == 2)
    {
        SetTimer2Baudraye(65536UL - (MAIN_Fosc / 4) / Baudrate2);

        S2CON &= ~(1<<7);   // 8位数据, 1位起始位, 1位停止位, 无校验
        IE2   |= 1;         //允许中断
        S2CON |= (1<<4);    //允许接收
        P_SW2 &= ~0x01; 
        P_SW2 |= 0;         //UART2 switch to: 0: P1.0 P1.1,  1: P4.6 P4.7

        B_TX2_Busy = 0;
        TX2_Cnt = 0;
        RX2_Cnt = 0;
    }
}

//========================================================================
// 函数: void UART2_int (void) interrupt UART2_VECTOR
// 描述: UART2中断函数。
// 参数: nine.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
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
