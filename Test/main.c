#include "reg52.h"			
//�����Զ���ͷ�ļ�
#include "public.h"
#include "smg.h"
#include "ds18b20.h"
#include "keyValue.h"
void main()
{	
	P0=0x00;//Ĭ�Ϲر���ʾ�����   
	while(1)
	{	
		KeyDown();		   //�����жϺ���
	}									 
}
