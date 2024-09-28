#include "seg.h"

/* 
======================================================================================
Function:
�˺�����*seg_string�ַ�����ÿһλ������ת�����������ʾ�Ķ���
���磺char seg_string[2] = {0,1}; //Ҫ��ʾ0��1
			char seg_buf[2] = {0,0};    //������
			
			Led_Trans(seg_string,seg_buf);//ת��
			
			//ת�����
			seg_string[2] = {0,1};
			seg_buf[2] = {0xFC,0x60};
										0xFC����ܱ�ʾΪ0��0x60Ϊ1
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
�������ܺ�LED���ƺ���,����������˳������,��������ͬʱ����
*seg_buf:����ܶ�ѡ ��seg_buf[2] = {0xFC,0x60};0xFC����ܱ�ʾΪ0��0x60Ϊ1
seg_buf1:�Ż�ǿ���������ʾѡ��=0ʱ�Ż�ǿ����ʾ0��=1ʱ�Ż�ǿ����ʾ1.ѡ��Χֻ��0-3
seg_buf2:�����·���LED��ʾ���أ���λ�жϣ��ɾ�ȷ����ÿһλLED ����������ֲ
���
seg_led = 2b: 0 0 1  1 	1  1  1  1     =1�� =0��
						      ��  �� ͯ �� �� ��
							    ��  �� �� ˮ ˮ ��
							    ʱ  ǿ
                  ��  ��
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
				seg_string = 0xE0; //�Ż�ʱ��LED
			else 
				seg_string = 0x00; //�Ż�ʱ��LED
			
			if(((seg_buf2 >> 3) & 0x01))
				seg_string |= 0x18; //ͯ��LED
			else 
				seg_string &= ~0x18; //ͯ��LED
			
			if(((seg_buf2 >> 2) & 0x01))
				seg_string |= 0x06; //��ˮ��LED
			else 
				seg_string &= ~0x06; //��ˮ��LED
				
			break;
		}
		case 4:{
			COM2 = 0;
			DIG1 = DIG2 = DIG3 = COM1 = 1;
			
			if(((seg_buf2 >> 4) & 0x01))
				seg_string = 0xE0; //�Ż�ǿ��LED
			else 
				seg_string = 0x00; //�Ż�ǿ��LED
			
			if(((seg_buf2 >> 1) & 0x01))
				seg_string |= 0x18; //��ˮLED
			else 
				seg_string &= ~0x18; //��ˮLED
			
			if(seg_buf2 & 0x01)
				seg_string |= 0x06; //�Ż�LED
			else 
				seg_string &= ~0x06; //�Ż�LED
			
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