/*

Version: 1.9.3
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
#include "eeprom.h"
#include "config.h"

//-----------功能选择区-----------
#define BUZZER_SWITCH  1  //蜂鸣器 0关闭 1开启
#define SOFT_RESET     0  //每24小时软件复位一次,初始化所有除了EEPROM外的变量 0关闭 1开启

#define SOFT_RESET_DLTIME  10 //按照当前烧录时间来修改,以确保每次软复位的时间都固定在晚上12点
														 //如当前是下午15点烧录,应设置为 15
//程序执行函数
void Key_Proc(void);   	   //Keystroke process function
void Disp_Proc(void);  	   //LCD Dsiplay process function

//自定义功能函数 
void Delay(unsigned int delay); //定时器延时 会卡住当前函数，但不会影响整个代码
void Buzzer_3time(bit sw);
void Buzzer_Ctrl(u16 tick);//蜂鸣器控制函数
void Peripheral_Init(void);//外设初始化函数

//----------------可能需要修改的变量-------------------------------------

//磁化电机挡位切换,可在此处修改不同挡位的占空比.
u8 pwm_duty[4] = {10,4,3,2}; //磁化挡位占空比选择 1档:40% 2档:30% 3档:20% 停止:10%
u8 pwm_index = 0;//占空比选择函数

bit WSWITCH = 0; //出水口开关 =1出水 =0关水

//刷新计数,按键10ms扫描一次,数码管200ms刷新一次
u16 key_slow_down = 0; //按键刷新计数
u16 disp_slow_down = 0; //LCD刷新计数

u16 key_tick; //long key press count
bit key_sw = 1;
bit keylong = 1;

#if SOFT_RESET  
u16 reset_tick_min = SOFT_RESET_DLTIME*60;		//调整软件复位烧录时间
u16 reset_tickwait = 0;   //如果在工作状态就记下超出的时间
u8 reset_tick_sec = 0;    //软件复位分钟计数
#endif

u8 mode_sw_tick;        //上电一分钟内可以切换高原模式计时
bit mode_sw;            //高原模式可切换标志位,=1时可以切换模式
bit mode_sw_tick_sw;    //开始计时标志位

u8 key_value = 0;				//按键值
u8 key_down;		 				//按键下降沿
u8 key_up;   		 				//上升沿
u8 key_old;  		 				//上次的按键值

u16 delay_tick;         //软件延时计数
u16 buzzer_tick;        //蜂鸣器短响计数
bit Buzzer3timeSW = 0;  //蜂鸣器连响三次

u16 countdown = 60;    	//倒计时60min
u16 sec_tick; 				 	//秒倒计时
u16 min_tick; 				 	//分钟计时
bit magnetize_tick_sw; 	//磁化水倒计时开关
u8 WSWITCH_tick = 0;    //开水两分钟后自动关水计数
bit temperature = 0;    //出水温度

u16 valve_delay;				//继电器延迟计数
u16 hot_delay;  				//开热水延迟开水
u16 hot_delaytime = 2000;//热水出水延迟时间ms
bit hot_delaysw;

u16 flow_tick;  				//流量计延迟关水
bit save_flag = 1;			//缺水保护标志位
u16 save_flagtick;

xdata u32 freq = 0; 							//流量计读取回来的频率
u16 freq_tick;					//频率计数
u16 freq_timetick;      //频率测量周期
u16 valve_tick;					//电磁阀延迟关闭计数
u16 pwm_tick;           //pwm计数

bit magnetize_sw = 0; 	//磁化水开关 1开 0关

char seg_string[2];			//数码管显示字符串
char seg_buf[2];				//数码管显示缓冲区
bit mode_disp;  				//模式显示,显示1时高原模式,显示0时普通模式
u16 mode_disptick;

u8 seg_buf2;						//按键下LED控制 0b0011 1111 1为亮 0为灭
u8 pos = 0;  						//数码管段选
u8 mode_buf = 0; 				//模式切换eeprom缓冲区

void main(){
	GPIO_Init();			//引脚初始化
	EXTI_Init();			//外部中断配置
	Timer1_Init();		//定时器初始化
	EA = 1;         	//打开总中断
	Config_Init();  	//初始化配置
	Peripheral_Init();//外设初始状态
	
	while(1)
	{
		Key_Proc();
		Disp_Proc();
	}
} 

//================中断函数=======================
void Timer1_Isr(void) interrupt 3   //1ms 中断一次
{
	//本中断函数内全是赋值，没有运算，每次中断时间不会过长
	if(++key_slow_down == 10) key_slow_down = 0;   //10ms扫描一次按键
	if(++disp_slow_down == 200) disp_slow_down = 0;//200ms刷新一次数码管
	 
//--------------------1s定时器---------------------------
	if(++sec_tick >= 1000){
		sec_tick = 0;
		
		//磁化时间显示控制
		if(magnetize_tick_sw){		
			if(++min_tick >= 60)
			{
				min_tick = 0;
				if(--countdown == 0){
					magnetize_tick_sw = 0;//计数关闭
					countdown = 60;//倒计时复位为60
					pwm_index = 0;								
					seg_buf2 &= ~0x01;//磁化LED关
					CHLED = 1;       //灯圈关
				}
			}
		}else 
			min_tick = 0;
		
		//开水2min后自动关水		
		if(WSWITCH){
			if(++WSWITCH_tick >= 120){ //120s
				WSWITCH_tick = 0;
				WSWITCH = 0;  //关水
				Valve = 0;    //关电磁阀
				seg_buf2 &= ~0x04; //关开水LED
				seg_buf2 &= ~0x02; //关冷水LED
				Buzzer_Ctrl(200); 	//蜂鸣器响200ms
			}
		}
		else 
			WSWITCH_tick = 0;
		
	#if SOFT_RESET  
		//24hour后自动复位初始化所有标志位和变量,提高工程稳定性
		if(++reset_tick_sec >= 60){
			reset_tick_min++;
			reset_tick_sec = 0;				  //分钟计数清零
			if(reset_tick_min >= 1440){ //60min*24h 每24h复位一次 
				  if(WSWITCH == 0 && magnetize_tick_sw == 0){//不在出水状态的时候and不在磁化倒计时的时候
						IAP_CONTR = 0x60; 		//单片机软复位寄存器,初始化所有变量
						reset_tick_min = 0; 	//计数器清零
						reset_tick_min = reset_tickwait;//保证每次复位都是在固定的时间
						reset_tickwait = 0;		//超出量
					}else{
						reset_tickwait++;			//超出量
					}
			}
		}
	#endif
		
		//开机1分钟内可以切换高原模式和普通模式
		if(!mode_sw_tick_sw){
			mode_sw = 1;//=1时可以切换高原模式
			mode_sw_tick++; 
			if(mode_sw_tick >= 60){//60s计数
				mode_sw_tick = 0;
				mode_sw_tick_sw = 1;
				mode_sw = 0;//=0时不允许切换模式
			}
		}
		

	}

//--------------------1s定时器---------------------------
//--------------------外设控制---------------------------
	
	//按键长短按计时
	if(key_tick > 0) key_tick--;  
	
	//浮点数运算 影响不大 下次优化时尽量放在中断外
	//频率测量
	if(++freq_timetick > 100){
		freq_timetick = 0;
		freq = ((u32)freq_tick / 2.0)*10.0;//单位Hz
		freq_tick = 0;
	}
	
	//刚按按键5s不检测流量计数
	if(save_flag == 0){
		if(++save_flagtick >= 5000){
			save_flagtick = 0;
			save_flag = 1;
		}
	}else 
		save_flagtick = 0;
	
	//流量计检测到如果流量低于某个值就关水，防干烧，刚开水两秒不检测
	if(WSWITCH && freq <= 5 && save_flag){
		if(++flow_tick > 100 && freq <= 5){
			flow_tick = 0;
			WSWITCH = 0;
			Valve = 0;
			hot_delaysw = 0;
			hot_delaytime = 2000;//触发保护后下一次出冷水时间调整为2s
			seg_buf2 &= ~0x04; //关开水LED
			seg_buf2 &= ~0x02; //关冷水LED
			Buzzer3timeSW = 1;
		}
	}else 
		flow_tick = 0;
	
	//每次开热水先开2秒冷水,并且有流量才给出热水
	//触发缺水保护之后先开3秒冷水
	if(hot_delaysw){
			Valve = 1;
			hot_delay++;
		
			if((hot_delay >= hot_delaytime) && (hot_delay < (hot_delaytime+100)))
				WSWITCH = 0;  //关水 要先关水并延时才能控制温度
			else if(hot_delay >= hot_delaytime+100){
				if(freq >= 3){
					hot_delay = 0;			 //计数清零
					hot_delaytime = 2000;//出冷水时间调整为1s
					hot_delaysw = 0;
					WSWITCH = 1;  //开水
					temperature = HOT;
				}else{
					hot_delay = 0;//计数清零
					hot_delaysw = 0;
					hot_delaytime = 3000;//出冷水时间调整为3s
					WSWITCH = 0;  //关水
					Valve = 0;
					seg_buf2 &= ~0x04; //关开水LED
					seg_buf2 &= ~0x02; //关冷水LED
					//temperature = HOT;
					Buzzer_Ctrl(1000);//蜂鸣器长响
				}
				
			}
	}else 
		hot_delay = 0;
	
	
		
	//磁化电机pwm
	if(pwm_index){
		if(++pwm_tick >= 10) pwm_tick = 0;//软件模拟PWM（stc8c用pca中断pwm 那还不如用软件模拟）
		if(pwm_tick < pwm_duty[pwm_index]) PWM = 1;
		else 	PWM = 0;
	}
	else 
		PWM = 1;
	
	//软件延时
	if(delay_tick > 0) delay_tick--;//延时函数 使用会卡住当前函数，但不影响其他功能
	
	
	//蜂鸣器定时器
	#if BUZZER_SWITCH  		
	if(buzzer_tick > 0){ 		//蜂鸣器短响时间计数				
		buzzer_tick--;
		Buzzer = 0;
	}else
		Buzzer = 1;
	#endif
	
	//数码管段选刷新
	Seg_Disp(seg_buf,pwm_index,seg_buf2,pos);
	if(++pos > 4) pos = 0;
	
	//电磁阀延迟关闭和开启
	if(WSWITCH){
		if(++valve_delay == 300){
			valve_delay = 0;
			Valve = 1;
		}
	}else{
		if(++valve_delay == 50){
			valve_delay = 0;
			Valve = 0;
		}
	}

	//开关热水冷水
	Faucet_Switch(temperature,WSWITCH);
	
	//高原模式和普通模式切换
	Mode = !mode_buf;
	
	//切换高原模式时显示一秒
	if(mode_disp){
		if(++mode_disptick >= 3000){
			mode_disptick = 0;
			mode_disp = 0;
		}
	}else 
		mode_disptick = 0;
		
		
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
	
	if(key_down)       //长按2秒
		key_tick = 3000;
	
	 //key_old键值 tick按下计数  long自锁，保证按键代码只触发一次
	if(key_old && key_tick == 0 && keylong){
		keylong = 0;		
		if(key_old == 1){ 		//长按童锁键		
			//------按键1长按功能--------
			WSWITCH = 0;        //出水开关关闭
			key_sw ^= 1; 				//=1时其他按键正常工作，=0时其他按键无法按下
			seg_buf2 ^= 0x08; 	//童锁LED
			seg_buf2 &= ~0x04; //关开水LED
			seg_buf2 &= ~0x02; //关冷水LED
			Buzzer_Ctrl(200); 	//蜂鸣器响200ms		
			
			//------按键1长按功能--------				
		}
		if(key_old == 4 && key_sw && mode_sw){ //长按磁化键	
			//------按键4长按功能--------
			mode_buf = !mode_buf;//长按磁化键更换高原模式
			
			mode_disp = 1; //数码管显示一下当前模式
			IapErase(IAP_ADDRESS);
			IapProgram(IAP_ADDRESS,mode_buf);
			Buzzer3timeSW = 1; //蜂鸣器响三下

			//------按键4长按功能--------		
		}
	}
	
	if(key_up) //keydown-keyup顺序不能更改
		keylong = 1;
	
	if(key_tick && key_sw){
		switch(key_up)
		{
			case 1://儿童锁键
			{ 		
				break;
			}
			case 2://热水按键
			{
				WSWITCH ^= 1;	//开关水
				save_flagtick = 0;
				save_flag = 0;//先不检测流量
				//Valve = WSWITCH; //电磁阀开关
				
				temperature = COLD;
				hot_delaysw = WSWITCH;
				
				seg_buf2 &= ~0x02; //关冷水LED
				Buzzer_Ctrl(200);//蜂鸣器响200ms
				
				if(WSWITCH)
					seg_buf2 |= 0x04; //开水LED开
				else
					seg_buf2 &= ~0x04; //关开水LED
				break;
			}
			case 3://冷水按键
			{
				WSWITCH ^= 1;
				save_flagtick = 0;
				save_flag = 0;//先不检测流量
				//Valve = WSWITCH; //电磁阀开关
				hot_delaysw = 0;
				temperature = COLD;
				seg_buf2 &= ~0x04; //关开水LED
				Buzzer_Ctrl(200);//蜂鸣器响200ms
				
				if(WSWITCH)
					seg_buf2 |= 0x02; //冷水LED开
				else
					seg_buf2 &= ~0x02; //关冷水LED
				break;
			}
			case 4://磁化按键
			{
				magnetize_tick_sw = 1;//开始倒计时
				CHLED = 0;       //灯圈开
				seg_buf2 |= 0x01;//磁化LED开
				Buzzer_Ctrl(200);//蜂鸣器响200ms 
				if(++pwm_index > 3){							//磁化强度选择 index = 0[Uenabled]
					magnetize_tick_sw = 0;					//                     1[duty30%]
					pwm_index = 0;									//                     2[duty20%]						
					seg_buf2 &= ~0x01;//磁化LED关
					CHLED = 1;       //灯圈关
					countdown = 60;//倒计时复位为60		
				}                      					  //                     3[duty10%]
				break;                             
			}                            				 
			default:                             
				break;
		}
	}
	
	Buzzer_3time(Buzzer3timeSW);
	
}

//================显示逻辑函数=======================
void Disp_Proc(void)  	   //LCD Dsiplay process function
{
	if(disp_slow_down) return;   //200ms进入一次此函数(200ms刷新一次屏幕)
		disp_slow_down = 1;
	
	
//	sprintf(seg_string,"%02d",(unsigned int)save_flag); //调试
	if(!mode_disp)
		sprintf(seg_string,"%2d",countdown);//显示磁化倒计时
	else 	
		sprintf(seg_string,"%2d",(unsigned int)Mode);//显示模式 1=高原模式，出水85°，0=普通模式，出水93°
	
	Led_Trans(seg_string,seg_buf);
		
}

//================外部中断函数=======================
void common_isr() interrupt 13
{
	u8 psw2_st;
	u8 intf;
	psw2_st = P_SW2;
	P_SW2 |= 0x80;
	intf = P0INTF;
	if (intf)
	{
		P0INTF = 0x00;		
		if (intf & 0x10)
		{
			//P0.4 口中断
			freq_tick += 1;
		}
		
		}
		P_SW2 = psw2_st;
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

void Buzzer_3time(bit sw)
{
	if(sw){
		Buzzer_Ctrl(100);
		Delay(200);
		Buzzer_Ctrl(100);
		Delay(200);
		Buzzer_Ctrl(100);
		Buzzer3timeSW = 0;
	} 	
}

void Peripheral_Init(void)//外设初始化函数
{
	Buzzer_Ctrl(200);//上电短响200ms一次
	seg_buf2 = 0x30;
 
	mode_buf = IapRead(IAP_ADDRESS); //读取EEPROM的值，模式默认为普通模式 0为普通模式 1为高原模式
}

