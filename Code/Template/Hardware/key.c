#include "key.h"

unsigned char Key_Scan(void)
{
	u8 key_value = 0;
	
	if(key1 == 0) //ͯ��
		return 1;
	if(key2 == 0) //��ˮ
		return 2;
	if(key3 == 0) //����ˮ
		return 3;
	if(key4 == 0) //�Ż�
		return 4;
	
	return key_value;
}


