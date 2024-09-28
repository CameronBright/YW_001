#include "key.h"

unsigned char Key_Scan(void)
{
	u8 key_value = 0;
	
	if(key1 == 0) //童锁
		return 1;
	if(key2 == 0) //开水
		return 2;
	if(key3 == 0) //常温水
		return 3;
	if(key4 == 0) //磁化
		return 4;
	
	return key_value;
}


