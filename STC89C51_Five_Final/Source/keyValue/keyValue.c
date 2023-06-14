//����ͷ�ļ�
#include "public.h"
#include "smg.h"
#include "ds18b20.h"
#include "keyValue.h"

unsigned int Speed,Compare;//���ȵ�λ��Compare�������жϺ���������������PWM 
//���󰴼�
unsigned int g,s,b,q,count=61,cd=1,flag_lis1=0,list1_falg=0;
/*����ʮ���١�ǧ������ʱ��ģʽ�л���
��һ��ֻ�а���S4��S5��S6����Ч���ܿ��أ���
����S6���²��ܰ�S5��S6��Ҫ�ٰ�һ��S4�򿪲�
�ܰ���
����S7���º��ڰ�S5��cd�����ӣ������ܲ��л�
*/
//��ʱ�жϳ�ʼ��
void time0_init(){
    TMOD&=0xf0;// ��� TMOD �Ĵ�������λ��ֵ
	TMOD|=0x01;// ���� TMOD �Ĵ�������λ��ֵΪ 0001����ʾ��ʱ�� 0 Ϊģʽ 1
    TH0=0xfc;;// ���ö�ʱ�� 0 �ĸ�λ��������ֵΪ 0xfc
    TL0=0x66;// ���ö�ʱ�� 0 �ĵ�λ��������ֵΪ 0x66
    TR0=1;// ������ʱ�� 0
    TF0=0; // �����ʱ�� 0 �����־λ
	EA=1;// �����ж��ܿ���
	ET0=1;// ����ʱ�� 0 �ж�
}
//��ʱ�жϺ���
void time0_isr()interrupt 1{
	static unsigned int i=0;//����ʱ
	TH0=0xfc;// ���ö�ʱ�� 0 �ĸ�λ��������ֵΪ 0xfc
	TL0=0x66;// ���ö�ʱ�� 0 �ĵ�λ��������ֵΪ 0x66
	//����ʱÿ���ȡ���٣�����ȡ����ʮ���١�ǧ������
	if(++i>1000){
		i=0;//���㣬��ֹ��ֵ���
		--count;//����ʱ����
		g=count%10;//��
		s=count/10%10;//ʮ
		b=count/100%10;//��
		q=count/1000;//ǧ
	}
	if(count==0)return;//����ʱ����ʱ�˳��жϺ���
	//���������ٶ�  ����Sped�󶨵�cd���������������ֱ���20��30��70��PWMռ�ձ�
	Compare++;
    if(Compare<Speed)                           
    {
        Motor=1;
    }
    else
    {
        Motor=0;
    }
    Compare%=100;
	 if(cd>3)cd=1;//��������1
}
//����ģ�麯��
void list(unsigned char num){
		u8 i=0;//���ڶ�ȡ�¶�ֵ
	   	int temp_value;//���ڴ洢�¶�ֵ 
		u8 temp_buf[8];//һ��Ҫ��ʾ8������ܵ�ֵ�������ҷֱ���0-7
		ds18b20_init();//��ʼ��DS18B20
		
		if(count>120)count=0;//����ʱ����120����
		while(1)//ѭ���Զ�ˢ���������ʾ�����һֱת��
		{			
			i++;//����
			if(i>4)i=0;//��ֹ��ֵ���
			if(i%2==0)//���һ��ʱ���ȡ�¶�ֵ�����ʱ��Ҫ�����¶ȴ�����ת���¶�ʱ��
				temp_value=ds18b20_read_temperture()*10;//�����¶�ֵС����һλ
			//����1-3
			if(count>=0){
				temp_buf[0]=0x40;//- 
				temp_buf[1]=gsmg_code[cd];//��������
				temp_buf[2]=0x40;//-
				temp_buf[3]=0x00;//Ϩ��
				temp_buf[4]=0x00;//Ϩ��
				temp_buf[5]=gsmg_code[b];//��λ
				temp_buf[6]=gsmg_code[s];//ʮλ
				temp_buf[7]=gsmg_code[g];//��λ
			}
			//����4
			if(num==4){
				temp_buf[0]=0x40;//- 
				temp_buf[1]=gsmg_code[4];
				temp_buf[2]=0x40;//-
				temp_buf[3]=0x00;//Ϩ��
				temp_buf[4]=0x00;//Ϩ��
				temp_buf[5]=gsmg_code[temp_value%1000/100];//ʮλ
				temp_buf[6]=gsmg_code[temp_value%1000%100/10];//��λ
				temp_buf[7]=0x39;//C
			}
			//count����ʱ���������
			if(num!=4&&count==0){
				temp_buf[0]=0x40;//- 
				temp_buf[1]=gsmg_code[cd];
				temp_buf[2]=0x40;//-
				temp_buf[3]=0x00;//Ϩ��
				temp_buf[4]=0x00;//Ϩ��
				temp_buf[5]=0x7f;//8
				temp_buf[6]=0x7f;//8
				temp_buf[7]=0x7f;//8
			}
			smg_display(temp_buf,1);//����ߵ�һ����ʼ��ʾ
			if(i%2==0)return ;//ÿ�μ���˳���ֹ��������
		}
}

#define GPIO_DIG P0//�������������
#define GPIO_KEY P1//������󰴼�����


u8 KeyValue;	//������Ŷ�ȡ���ľ��󰸼��ļ�ֵ
//��ֵ��ȡ����
void KeyDown()
{
	GPIO_KEY=0x0f;//������ʼ��
	if(GPIO_KEY!=0x0f)//��ȡ�����Ƿ���
	{
		delay_10us(1000);//��ʱ10ms��������
		if(GPIO_KEY!=0x0f)//�ٴμ������Ƿ���
		{	
			//������
			GPIO_KEY=0X0F;
			switch(GPIO_KEY)
			{
				case(0X07):	KeyValue=0;break;//������ֵ0-3��һ�����ĸ�����
				case(0X0b):	KeyValue=1;break;
				case(0X0d): KeyValue=2;break;
				case(0X0e):	KeyValue=3;break;
			}
			//������
			GPIO_KEY=0XF0;
			switch(GPIO_KEY)
			{
				case(0X70):	KeyValue=KeyValue;break;//��ֵ��������0��4��8��12
				case(0Xb0):	KeyValue=KeyValue+4;break;
				case(0Xd0): KeyValue=KeyValue+8;break;
				case(0Xe0):	KeyValue=KeyValue+12;break;
				/*
					�����ȡ���ǵ��������еļ�ֵ
					���ǵ����м�0xd0����ֵΪ8
					���ǵ����м�0xe0,��ֵΪ3
					8+3=12����Ϊ���㿪ʼ���Ի�Ҫ��ȥ1���������������
					���������еļ�ֵΪ11
				*/
			}
			
		}
	}
	Motor=0;//�رշ���
	time0_init();//��ʱ�жϳ�ʼ��	   
	 
	if(KeyValue==3){//����1�л�����1-3	
		flag_lis1=1;//��S5��S6�İ�������
		if(cd==1)Speed=20;//���ݹ��ܰ�PWMռ��
		if(cd==2)Speed=30;
		if(cd==3)Speed=70;
		list(1);
	}
	if(KeyValue==4){//����2 ����60s	����ʱ
		if(flag_lis1)list(2);//flag_lis1��ʼ�����ã�Ҫ����S4������Ч	   
	}
	if(KeyValue==5){//����3  ���㵹��ʱ
		if(flag_lis1)list(3);//flag_lis1��ʼ�����ã�Ҫ����S4������Ч
	}
	if(KeyValue==6){//����4	 ����
		list(4);
	} 
	//��ⰴ�����ּ��
	if(GPIO_KEY==0xf0&&KeyValue==3)	
	{				
		if(list1_falg)cd+=1;//����������ʾ�л�
		
		list1_falg=1;//�����ӹ����л�
		if(cd>3)cd=1;//����ֻ��1-3	
	}
	//����ʱ���ӻ����
	if(GPIO_KEY==0xf0&&KeyValue==4)
	{
		if(flag_lis1)count+=60;
		/*
			����60s ����ʱ	ֻ�а���S4�򿪿��غ���Ч 
			�����˺����ֱ�Ӱ����������  ��������ʱ�ٹر�
		*/
	}
	if(GPIO_KEY==0xf0&&KeyValue==5)
	{
		if(flag_lis1)count=0;//����	 ����ʱ	 ֻ�а���S4�򿪿��غ���Ч
	}
	if(GPIO_KEY==0xf0&&KeyValue==6)	
	{
	  	flag_lis1=0;//�ر�S5-S6����
		list1_falg=0;//�ر�cd�������л�
	}
}