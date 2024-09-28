/*

Version: 0.0.0
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
//#include "eeprom.h"
#include "config.h"

//-----------功能选择区-----------


//程序执行函数
void Key_Proc(void);   	   //Keystroke process function
void Disp_Proc(void);  	   //LCD Dsiplay process function

//自定义功能函数 
void Delay(unsigned int delay); //定时器延时 会卡住当前函数，但不会影响整个代码
void Peripheral_Init(void);//外设初始化函数

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
char seg_string[2];			//数码管显示字符串
char seg_buf[2];				//数码管显示缓冲区

u8 seg_buf2;						//按键下LED控制 0b0011 1111 1为亮 0为灭
u8 pos1 = 0;  						//数码管段选
u8 pos2 = 0;  						//数码管段选

/*外设相关变量*/
u16 buzzer_tick;        //蜂鸣器短响计数

void main(){
	GPIO_Init();			//引脚初始化
	Timer1_Init();		//定时器初始化
	EA = 1;         	//打开总中断
	Config_Init();  	//初始化配置
	Peripheral_Init();//外设函数初始状态
	
	while(1)
	{
		Key_Proc();
		Disp_Proc();
		LED1 = 0; 
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
//	Seg_Disp(seg_buf,pwm_index,seg_buf2,pos);
	if(++pos1 > 3) pos1 = 0;
	if(++pos2 > 3) pos2 = 0;
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
			case 1://锁键1
			{ 		
				LED1 ^=1;
				break;
			}
			case 2://按键2
			{
				LED2 ^= 1;
				break;
			}
			case 3://按键3
			{
				LED3 ^= 1;
				break;
			}
			case 4://按键4
			{    					
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
	
	
//	sprintf(seg_string,"%02d",(unsigned int)save_flag); //调试
	 	
//	sprintf(seg_string,"%2d",(unsigned int)Mode);//显示模式 1=高原模式，出水85°，0=普通模式，出水93°
	
//	Led_Trans(seg_string,seg_buf);
		
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
	seg_buf2 = 0x30;
 
}

