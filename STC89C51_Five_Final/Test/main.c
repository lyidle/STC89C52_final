#include "reg52.h"			
//引入自定义头文件
#include "public.h"
#include "smg.h"
#include "ds18b20.h"
#include "keyValue.h"
void main()
{	
	P0=0x00;//默认关闭显示数码管   
	while(1)
	{	
		KeyDown();		   //按键判断函数
	}									 
}
