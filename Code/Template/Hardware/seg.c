#include "seg.h"

/* 
======================================================================================
Function:
此函数将*seg_string字符串中每一位的内容转换成数码管显示的段码
例如：char seg_string[2] = {0,1}; //要显示0和1
			char seg_buf[2] = {0,0};    //缓冲区
			
			Led_Trans(seg_string,seg_buf);//转换
			
			//转换结果
			seg_string[2] = {0,1};
			seg_buf[2] = {0xFC,0x60};
										0xFC数码管表示为0，0x60为1
======================================================================================	
*/
void Led_Trans(unsigned char *seg_string,unsigned char *seg_buf)
{
	unsigned char i,j = 0;
	unsigned char temp;
	
	for (i=0;i<2;i++,j++)
	{
		switch(seg_string[j]){
			case '0': temp = 0xFC; break;
			case '1': temp = 0x60; break;
			case '2': temp = 0xDA; break;
			case '3': temp = 0xF2; break;
			case '4': temp = 0x66; break;
			case '5': temp = 0xB6; break;
			case '6': temp = 0xBE; break;
			case '7': temp = 0xE0; break;
			case '8': temp = 0xFF; break;
			case '9': temp = 0xF6; break;
			default:  temp = 0xFC; break;
		}
		seg_buf[i] = temp;
	}
	
}

/*
======================================================================================
多个数码管和LED控制函数,适用于引脚顺序不连贯,多个数码管同时控制
*seg_buf:数码管段选 例seg_buf[2] = {0xFC,0x60};0xFC数码管表示为0，0x60为1
seg_buf1:磁化强度数码管显示选择，=0时磁化强度显示0，=1时磁化强度显示1.选择范围只有0-3
seg_buf2:按键下方的LED显示开关，按位判断，可精确控制每一位LED ，并方便移植
表格：
seg_led = 2b: 0 0 1  1 	1  1  1  1     =1开 =0关
						      磁  磁 童 开 冷 磁
							    化  化 锁 水 水 化
							    时  强
                  间  度
======================================================================================
*/

void Seg_Disp(unsigned char *seg_buf1,unsigned char *seg_buf2,unsigned char seg_led, unsigned char pos)
{
	unsigned char seg_string;
	
	switch(pos){	
		case 0:{		
			DIG1 = 0;
			DIG2 = DIG3 = COM1 = COM2 = 1;		
			seg_string = seg_buf[pos];	
			break;
		}
		case 1:{
			DIG2 = 0;
			DIG1 = DIG3 = COM1 = COM2 = 1;
			seg_string = seg_buf[pos];	
			break;
		}
		case 2:{
			DIG3 = 0;
			DIG1 = DIG2 = COM1 = COM2 = 1;
			switch(seg_buf1){
				case 0:
					seg_string = 0xFC; //0
					break;
				case 1:
					seg_string = 0x60; //1
					break;
				case 2:
					seg_string = 0xDA; //2
					break;
				case 3:
					seg_string = 0xF2; //3
					break;
				default: break;
			}
			break;
		}
		case 3:{
			COM1 = 0;
			DIG1 = DIG2 = DIG3 = COM2 = 1;
			
			if(((seg_buf2 >> 5) & 0x01))
				seg_string = 0xE0; //磁化时间LED
			else 
				seg_string = 0x00; //磁化时间LED
			
			if(((seg_buf2 >> 3) & 0x01))
				seg_string |= 0x18; //童锁LED
			else 
				seg_string &= ~0x18; //童锁LED
			
			if(((seg_buf2 >> 2) & 0x01))
				seg_string |= 0x06; //开水键LED
			else 
				seg_string &= ~0x06; //开水键LED
				
			break;
		}
		case 4:{
			COM2 = 0;
			DIG1 = DIG2 = DIG3 = COM1 = 1;
			
			if(((seg_buf2 >> 4) & 0x01))
				seg_string = 0xE0; //磁化强度LED
			else 
				seg_string = 0x00; //磁化强度LED
			
			if(((seg_buf2 >> 1) & 0x01))
				seg_string |= 0x18; //冷水LED
			else 
				seg_string &= ~0x18; //冷水LED
			
			if(seg_buf2 & 0x01)
				seg_string |= 0x06; //磁化LED
			else 
				seg_string &= ~0x06; //磁化LED
			
			break;
		}
		default: break;
	}
	
	
	LED1A = seg_string >> 7;
	LED1B = seg_string >> 6 & 0x01;
	LED1C = seg_string >> 5 & 0x01;
	LED1D = seg_string >> 4 & 0x01;
	LED1E = seg_string >> 3 & 0x01;
	LED1F = seg_string >> 2 & 0x01;
	LED1G = seg_string >> 1 & 0x01;
	
}

void Seg_Disp1(unsigned char *seg_buf, unsigned char pos)
{
	unsigned char seg_string;
	
	switch(pos){	
		case 0:{		
			ADIG1 = 0;
			ADIG2 = ADIG3 = ADIG4 = 1;		
			seg_string = seg_buf[pos];	
			break;
		}
		case 1:{
			ADIG2 = 0;
			ADIG1 = ADIG3 = ADIG4 = 1;
			seg_string = seg_buf[pos];	
			break;
		}
		case 2:{
			ADIG3 = 0;
			ADIG1 = ADIG2 = ADIG4 = 1;
			seg_string = seg_buf[pos];
			break;
		}
		case 3:{
			ADIG4 = 0;
			ADIG1 = ADIG2 = ADIG3 = 1;
			seg_string = seg_buf[pos];	
			break;
		}
		default: break;
	}

	LEDA = seg_string >> 7;
	LEDB = seg_string >> 6 & 0x01;
	LEDC = seg_string >> 5 & 0x01;
	LEDD = seg_string >> 4 & 0x01;
	LEDE = seg_string >> 3 & 0x01;
	LEDF = seg_string >> 2 & 0x01;
	LEDG = seg_string >> 1 & 0x01;
}