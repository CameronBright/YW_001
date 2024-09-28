#ifndef __EEPROM_H
#define __EEPROM_H

#include "main.h"

//void IapIdle();
//char IapRead(int addr);
//void IapProgram(int addr, char dat);
//void IapErase(int addr);

//使用方法

//读取
//mode_buf = IapRead(IAP_ADDRESS); //每次上电时再读取

//写入
//8字节，16字节的分开存
//mode_buf = ~mode_buf;
//IapErase(IAP_ADDRESS); //先清空 
//IapProgram(IAP_ADDRESS,mode_buf); //再写入

#endif
