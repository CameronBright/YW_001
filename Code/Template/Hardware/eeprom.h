#ifndef __EEPROM_H
#define __EEPROM_H

#include "main.h"

//void IapIdle();
//char IapRead(int addr);
//void IapProgram(int addr, char dat);
//void IapErase(int addr);

//ʹ�÷���

//��ȡ
//mode_buf = IapRead(IAP_ADDRESS); //ÿ���ϵ�ʱ�ٶ�ȡ

//д��
//8�ֽڣ�16�ֽڵķֿ���
//mode_buf = ~mode_buf;
//IapErase(IAP_ADDRESS); //����� 
//IapProgram(IAP_ADDRESS,mode_buf); //��д��

#endif
