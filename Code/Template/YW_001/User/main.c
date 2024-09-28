/*

Version: 1.9.3
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
#include "eeprom.h"
#include "config.h"

//-----------����ѡ����-----------
#define BUZZER_SWITCH  1  //������ 0�ر� 1����
#define SOFT_RESET     0  //ÿ24Сʱ�����λһ��,��ʼ�����г���EEPROM��ı��� 0�ر� 1����

#define SOFT_RESET_DLTIME  10 //���յ�ǰ��¼ʱ�����޸�,��ȷ��ÿ����λ��ʱ�䶼�̶�������12��
														 //�統ǰ������15����¼,Ӧ����Ϊ 15
//����ִ�к���
void Key_Proc(void);   	   //Keystroke process function
void Disp_Proc(void);  	   //LCD Dsiplay process function

//�Զ��幦�ܺ��� 
void Delay(unsigned int delay); //��ʱ����ʱ �Ῠס��ǰ������������Ӱ����������
void Buzzer_3time(bit sw);
void Buzzer_Ctrl(u16 tick);//���������ƺ���
void Peripheral_Init(void);//�����ʼ������

//----------------������Ҫ�޸ĵı���-------------------------------------

//�Ż������λ�л�,���ڴ˴��޸Ĳ�ͬ��λ��ռ�ձ�.
u8 pwm_duty[4] = {10,4,3,2}; //�Ż���λռ�ձ�ѡ�� 1��:40% 2��:30% 3��:20% ֹͣ:10%
u8 pwm_index = 0;//ռ�ձ�ѡ����

bit WSWITCH = 0; //��ˮ�ڿ��� =1��ˮ =0��ˮ

//ˢ�¼���,����10msɨ��һ��,�����200msˢ��һ��
u16 key_slow_down = 0; //����ˢ�¼���
u16 disp_slow_down = 0; //LCDˢ�¼���

u16 key_tick; //long key press count
bit key_sw = 1;
bit keylong = 1;

#if SOFT_RESET  
u16 reset_tick_min = SOFT_RESET_DLTIME*60;		//���������λ��¼ʱ��
u16 reset_tickwait = 0;   //����ڹ���״̬�ͼ��³�����ʱ��
u8 reset_tick_sec = 0;    //�����λ���Ӽ���
#endif

u8 mode_sw_tick;        //�ϵ�һ�����ڿ����л���ԭģʽ��ʱ
bit mode_sw;            //��ԭģʽ���л���־λ,=1ʱ�����л�ģʽ
bit mode_sw_tick_sw;    //��ʼ��ʱ��־λ

u8 key_value = 0;				//����ֵ
u8 key_down;		 				//�����½���
u8 key_up;   		 				//������
u8 key_old;  		 				//�ϴεİ���ֵ

u16 delay_tick;         //�����ʱ����
u16 buzzer_tick;        //�������������
bit Buzzer3timeSW = 0;  //��������������

u16 countdown = 60;    	//����ʱ60min
u16 sec_tick; 				 	//�뵹��ʱ
u16 min_tick; 				 	//���Ӽ�ʱ
bit magnetize_tick_sw; 	//�Ż�ˮ����ʱ����
u8 WSWITCH_tick = 0;    //��ˮ�����Ӻ��Զ���ˮ����
bit temperature = 0;    //��ˮ�¶�

u16 valve_delay;				//�̵����ӳټ���
u16 hot_delay;  				//����ˮ�ӳٿ�ˮ
u16 hot_delaytime = 2000;//��ˮ��ˮ�ӳ�ʱ��ms
bit hot_delaysw;

u16 flow_tick;  				//�������ӳٹ�ˮ
bit save_flag = 1;			//ȱˮ������־λ
u16 save_flagtick;

xdata u32 freq = 0; 							//�����ƶ�ȡ������Ƶ��
u16 freq_tick;					//Ƶ�ʼ���
u16 freq_timetick;      //Ƶ�ʲ�������
u16 valve_tick;					//��ŷ��ӳٹرռ���
u16 pwm_tick;           //pwm����

bit magnetize_sw = 0; 	//�Ż�ˮ���� 1�� 0��

char seg_string[2];			//�������ʾ�ַ���
char seg_buf[2];				//�������ʾ������
bit mode_disp;  				//ģʽ��ʾ,��ʾ1ʱ��ԭģʽ,��ʾ0ʱ��ͨģʽ
u16 mode_disptick;

u8 seg_buf2;						//������LED���� 0b0011 1111 1Ϊ�� 0Ϊ��
u8 pos = 0;  						//����ܶ�ѡ
u8 mode_buf = 0; 				//ģʽ�л�eeprom������

void main(){
	GPIO_Init();			//���ų�ʼ��
	EXTI_Init();			//�ⲿ�ж�����
	Timer1_Init();		//��ʱ����ʼ��
	EA = 1;         	//�����ж�
	Config_Init();  	//��ʼ������
	Peripheral_Init();//�����ʼ״̬
	
	while(1)
	{
		Key_Proc();
		Disp_Proc();
	}
} 

//================�жϺ���=======================
void Timer1_Isr(void) interrupt 3   //1ms �ж�һ��
{
	//���жϺ�����ȫ�Ǹ�ֵ��û�����㣬ÿ���ж�ʱ�䲻�����
	if(++key_slow_down == 10) key_slow_down = 0;   //10msɨ��һ�ΰ���
	if(++disp_slow_down == 200) disp_slow_down = 0;//200msˢ��һ�������
	 
//--------------------1s��ʱ��---------------------------
	if(++sec_tick >= 1000){
		sec_tick = 0;
		
		//�Ż�ʱ����ʾ����
		if(magnetize_tick_sw){		
			if(++min_tick >= 60)
			{
				min_tick = 0;
				if(--countdown == 0){
					magnetize_tick_sw = 0;//�����ر�
					countdown = 60;//����ʱ��λΪ60
					pwm_index = 0;								
					seg_buf2 &= ~0x01;//�Ż�LED��
					CHLED = 1;       //��Ȧ��
				}
			}
		}else 
			min_tick = 0;
		
		//��ˮ2min���Զ���ˮ		
		if(WSWITCH){
			if(++WSWITCH_tick >= 120){ //120s
				WSWITCH_tick = 0;
				WSWITCH = 0;  //��ˮ
				Valve = 0;    //�ص�ŷ�
				seg_buf2 &= ~0x04; //�ؿ�ˮLED
				seg_buf2 &= ~0x02; //����ˮLED
				Buzzer_Ctrl(200); 	//��������200ms
			}
		}
		else 
			WSWITCH_tick = 0;
		
	#if SOFT_RESET  
		//24hour���Զ���λ��ʼ�����б�־λ�ͱ���,��߹����ȶ���
		if(++reset_tick_sec >= 60){
			reset_tick_min++;
			reset_tick_sec = 0;				  //���Ӽ�������
			if(reset_tick_min >= 1440){ //60min*24h ÿ24h��λһ�� 
				  if(WSWITCH == 0 && magnetize_tick_sw == 0){//���ڳ�ˮ״̬��ʱ��and���ڴŻ�����ʱ��ʱ��
						IAP_CONTR = 0x60; 		//��Ƭ����λ�Ĵ���,��ʼ�����б���
						reset_tick_min = 0; 	//����������
						reset_tick_min = reset_tickwait;//��֤ÿ�θ�λ�����ڹ̶���ʱ��
						reset_tickwait = 0;		//������
					}else{
						reset_tickwait++;			//������
					}
			}
		}
	#endif
		
		//����1�����ڿ����л���ԭģʽ����ͨģʽ
		if(!mode_sw_tick_sw){
			mode_sw = 1;//=1ʱ�����л���ԭģʽ
			mode_sw_tick++; 
			if(mode_sw_tick >= 60){//60s����
				mode_sw_tick = 0;
				mode_sw_tick_sw = 1;
				mode_sw = 0;//=0ʱ�������л�ģʽ
			}
		}
		

	}

//--------------------1s��ʱ��---------------------------
//--------------------�������---------------------------
	
	//�������̰���ʱ
	if(key_tick > 0) key_tick--;  
	
	//���������� Ӱ�첻�� �´��Ż�ʱ���������ж���
	//Ƶ�ʲ���
	if(++freq_timetick > 100){
		freq_timetick = 0;
		freq = ((u32)freq_tick / 2.0)*10.0;//��λHz
		freq_tick = 0;
	}
	
	//�հ�����5s�������������
	if(save_flag == 0){
		if(++save_flagtick >= 5000){
			save_flagtick = 0;
			save_flag = 1;
		}
	}else 
		save_flagtick = 0;
	
	//�����Ƽ�⵽�����������ĳ��ֵ�͹�ˮ�������գ��տ�ˮ���벻���
	if(WSWITCH && freq <= 5 && save_flag){
		if(++flow_tick > 100 && freq <= 5){
			flow_tick = 0;
			WSWITCH = 0;
			Valve = 0;
			hot_delaysw = 0;
			hot_delaytime = 2000;//������������һ�γ���ˮʱ�����Ϊ2s
			seg_buf2 &= ~0x04; //�ؿ�ˮLED
			seg_buf2 &= ~0x02; //����ˮLED
			Buzzer3timeSW = 1;
		}
	}else 
		flow_tick = 0;
	
	//ÿ�ο���ˮ�ȿ�2����ˮ,�����������Ÿ�����ˮ
	//����ȱˮ����֮���ȿ�3����ˮ
	if(hot_delaysw){
			Valve = 1;
			hot_delay++;
		
			if((hot_delay >= hot_delaytime) && (hot_delay < (hot_delaytime+100)))
				WSWITCH = 0;  //��ˮ Ҫ�ȹ�ˮ����ʱ���ܿ����¶�
			else if(hot_delay >= hot_delaytime+100){
				if(freq >= 3){
					hot_delay = 0;			 //��������
					hot_delaytime = 2000;//����ˮʱ�����Ϊ1s
					hot_delaysw = 0;
					WSWITCH = 1;  //��ˮ
					temperature = HOT;
				}else{
					hot_delay = 0;//��������
					hot_delaysw = 0;
					hot_delaytime = 3000;//����ˮʱ�����Ϊ3s
					WSWITCH = 0;  //��ˮ
					Valve = 0;
					seg_buf2 &= ~0x04; //�ؿ�ˮLED
					seg_buf2 &= ~0x02; //����ˮLED
					//temperature = HOT;
					Buzzer_Ctrl(1000);//����������
				}
				
			}
	}else 
		hot_delay = 0;
	
	
		
	//�Ż����pwm
	if(pwm_index){
		if(++pwm_tick >= 10) pwm_tick = 0;//���ģ��PWM��stc8c��pca�ж�pwm �ǻ����������ģ�⣩
		if(pwm_tick < pwm_duty[pwm_index]) PWM = 1;
		else 	PWM = 0;
	}
	else 
		PWM = 1;
	
	//�����ʱ
	if(delay_tick > 0) delay_tick--;//��ʱ���� ʹ�ûῨס��ǰ����������Ӱ����������
	
	
	//��������ʱ��
	#if BUZZER_SWITCH  		
	if(buzzer_tick > 0){ 		//����������ʱ�����				
		buzzer_tick--;
		Buzzer = 0;
	}else
		Buzzer = 1;
	#endif
	
	//����ܶ�ѡˢ��
	Seg_Disp(seg_buf,pwm_index,seg_buf2,pos);
	if(++pos > 4) pos = 0;
	
	//��ŷ��ӳٹرպͿ���
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

	//������ˮ��ˮ
	Faucet_Switch(temperature,WSWITCH);
	
	//��ԭģʽ����ͨģʽ�л�
	Mode = !mode_buf;
	
	//�л���ԭģʽʱ��ʾһ��
	if(mode_disp){
		if(++mode_disptick >= 3000){
			mode_disptick = 0;
			mode_disp = 0;
		}
	}else 
		mode_disptick = 0;
		
		
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
	
	if(key_down)       //����2��
		key_tick = 3000;
	
	 //key_old��ֵ tick���¼���  long��������֤��������ֻ����һ��
	if(key_old && key_tick == 0 && keylong){
		keylong = 0;		
		if(key_old == 1){ 		//����ͯ����		
			//------����1��������--------
			WSWITCH = 0;        //��ˮ���عر�
			key_sw ^= 1; 				//=1ʱ������������������=0ʱ���������޷�����
			seg_buf2 ^= 0x08; 	//ͯ��LED
			seg_buf2 &= ~0x04; //�ؿ�ˮLED
			seg_buf2 &= ~0x02; //����ˮLED
			Buzzer_Ctrl(200); 	//��������200ms		
			
			//------����1��������--------				
		}
		if(key_old == 4 && key_sw && mode_sw){ //�����Ż���	
			//------����4��������--------
			mode_buf = !mode_buf;//�����Ż���������ԭģʽ
			
			mode_disp = 1; //�������ʾһ�µ�ǰģʽ
			IapErase(IAP_ADDRESS);
			IapProgram(IAP_ADDRESS,mode_buf);
			Buzzer3timeSW = 1; //������������

			//------����4��������--------		
		}
	}
	
	if(key_up) //keydown-keyup˳���ܸ���
		keylong = 1;
	
	if(key_tick && key_sw){
		switch(key_up)
		{
			case 1://��ͯ����
			{ 		
				break;
			}
			case 2://��ˮ����
			{
				WSWITCH ^= 1;	//����ˮ
				save_flagtick = 0;
				save_flag = 0;//�Ȳ��������
				//Valve = WSWITCH; //��ŷ�����
				
				temperature = COLD;
				hot_delaysw = WSWITCH;
				
				seg_buf2 &= ~0x02; //����ˮLED
				Buzzer_Ctrl(200);//��������200ms
				
				if(WSWITCH)
					seg_buf2 |= 0x04; //��ˮLED��
				else
					seg_buf2 &= ~0x04; //�ؿ�ˮLED
				break;
			}
			case 3://��ˮ����
			{
				WSWITCH ^= 1;
				save_flagtick = 0;
				save_flag = 0;//�Ȳ��������
				//Valve = WSWITCH; //��ŷ�����
				hot_delaysw = 0;
				temperature = COLD;
				seg_buf2 &= ~0x04; //�ؿ�ˮLED
				Buzzer_Ctrl(200);//��������200ms
				
				if(WSWITCH)
					seg_buf2 |= 0x02; //��ˮLED��
				else
					seg_buf2 &= ~0x02; //����ˮLED
				break;
			}
			case 4://�Ż�����
			{
				magnetize_tick_sw = 1;//��ʼ����ʱ
				CHLED = 0;       //��Ȧ��
				seg_buf2 |= 0x01;//�Ż�LED��
				Buzzer_Ctrl(200);//��������200ms 
				if(++pwm_index > 3){							//�Ż�ǿ��ѡ�� index = 0[Uenabled]
					magnetize_tick_sw = 0;					//                     1[duty30%]
					pwm_index = 0;									//                     2[duty20%]						
					seg_buf2 &= ~0x01;//�Ż�LED��
					CHLED = 1;       //��Ȧ��
					countdown = 60;//����ʱ��λΪ60		
				}                      					  //                     3[duty10%]
				break;                             
			}                            				 
			default:                             
				break;
		}
	}
	
	Buzzer_3time(Buzzer3timeSW);
	
}

//================��ʾ�߼�����=======================
void Disp_Proc(void)  	   //LCD Dsiplay process function
{
	if(disp_slow_down) return;   //200ms����һ�δ˺���(200msˢ��һ����Ļ)
		disp_slow_down = 1;
	
	
//	sprintf(seg_string,"%02d",(unsigned int)save_flag); //����
	if(!mode_disp)
		sprintf(seg_string,"%2d",countdown);//��ʾ�Ż�����ʱ
	else 	
		sprintf(seg_string,"%2d",(unsigned int)Mode);//��ʾģʽ 1=��ԭģʽ����ˮ85�㣬0=��ͨģʽ����ˮ93��
	
	Led_Trans(seg_string,seg_buf);
		
}

//================�ⲿ�жϺ���=======================
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
			//P0.4 ���ж�
			freq_tick += 1;
		}
		
		}
		P_SW2 = psw2_st;
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

void Peripheral_Init(void)//�����ʼ������
{
	Buzzer_Ctrl(200);//�ϵ����200msһ��
	seg_buf2 = 0x30;
 
	mode_buf = IapRead(IAP_ADDRESS); //��ȡEEPROM��ֵ��ģʽĬ��Ϊ��ͨģʽ 0Ϊ��ͨģʽ 1Ϊ��ԭģʽ
}

