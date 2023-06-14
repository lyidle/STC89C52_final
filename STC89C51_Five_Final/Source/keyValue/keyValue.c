//引入头文件
#include "public.h"
#include "smg.h"
#include "ds18b20.h"
#include "keyValue.h"

unsigned int Speed,Compare;//风扇挡位、Compare用于在中断函数当中自增调节PWM 
//矩阵按键
unsigned int g,s,b,q,count=61,cd=1,flag_lis1=0,list1_falg=0;
/*个、十、百、千、倒计时、模式切换、
第一次只有按了S4后S5、S6才生效（总开关），
按下S6室温不能按S5、S6需要再按一次S4打开才
能按、
按下S7室温后在按S5的cd不增加，即功能不切换
*/
//定时中断初始化
void time0_init(){
    TMOD&=0xf0;// 清除 TMOD 寄存器低四位的值
	TMOD|=0x01;// 设置 TMOD 寄存器低四位的值为 0001，表示定时器 0 为模式 1
    TH0=0xfc;;// 设置定时器 0 的高位计数器初值为 0xfc
    TL0=0x66;// 设置定时器 0 的低位计数器初值为 0x66
    TR0=1;// 启动定时器 0
    TF0=0; // 清除定时器 0 溢出标志位
	EA=1;// 允许中断总开关
	ET0=1;// 允许定时器 0 中断
}
//定时中断函数
void time0_isr()interrupt 1{
	static unsigned int i=0;//倒计时
	TH0=0xfc;// 设置定时器 0 的高位计数器初值为 0xfc
	TL0=0x66;// 设置定时器 0 的低位计数器初值为 0x66
	//倒计时每秒获取减少，并获取个、十、百、千的数字
	if(++i>1000){
		i=0;//归零，防止数值溢出
		--count;//倒计时减少
		g=count%10;//个
		s=count/10%10;//十
		b=count/100%10;//百
		q=count/1000;//千
	}
	if(count==0)return;//倒计时归零时退出中断函数
	//风扇运行速度  根据Sped绑定的cd即功能来决定，分别是20、30、70的PWM占空比
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
	 if(cd>3)cd=1;//超出三归1
}
//功能模块函数
void list(unsigned char num){
		u8 i=0;//用于读取温度值
	   	int temp_value;//用于存储温度值 
		u8 temp_buf[8];//一共要显示8个数码管的值，从左到右分别是0-7
		ds18b20_init();//初始化DS18B20
		
		if(count>120)count=0;//倒计时超出120归零
		while(1)//循环自动刷新数码管显示，电机一直转动
		{			
			i++;//自增
			if(i>4)i=0;//防止数值溢出
			if(i%2==0)//间隔一段时间读取温度值，间隔时间要大于温度传感器转换温度时间
				temp_value=ds18b20_read_temperture()*10;//保留温度值小数后一位
			//功能1-3
			if(count>=0){
				temp_buf[0]=0x40;//- 
				temp_buf[1]=gsmg_code[cd];//功能数字
				temp_buf[2]=0x40;//-
				temp_buf[3]=0x00;//熄灭
				temp_buf[4]=0x00;//熄灭
				temp_buf[5]=gsmg_code[b];//百位
				temp_buf[6]=gsmg_code[s];//十位
				temp_buf[7]=gsmg_code[g];//个位
			}
			//功能4
			if(num==4){
				temp_buf[0]=0x40;//- 
				temp_buf[1]=gsmg_code[4];
				temp_buf[2]=0x40;//-
				temp_buf[3]=0x00;//熄灭
				temp_buf[4]=0x00;//熄灭
				temp_buf[5]=gsmg_code[temp_value%1000/100];//十位
				temp_buf[6]=gsmg_code[temp_value%1000%100/10];//个位
				temp_buf[7]=0x39;//C
			}
			//count倒计时归零数码管
			if(num!=4&&count==0){
				temp_buf[0]=0x40;//- 
				temp_buf[1]=gsmg_code[cd];
				temp_buf[2]=0x40;//-
				temp_buf[3]=0x00;//熄灭
				temp_buf[4]=0x00;//熄灭
				temp_buf[5]=0x7f;//8
				temp_buf[6]=0x7f;//8
				temp_buf[7]=0x7f;//8
			}
			smg_display(temp_buf,1);//从左边第一个开始显示
			if(i%2==0)return ;//每次间隔退出防止按键锁死
		}
}

#define GPIO_DIG P0//定义数码管引脚
#define GPIO_KEY P1//定义矩阵按键引脚


u8 KeyValue;	//用来存放读取到的矩阵案件的键值
//键值获取函数
void KeyDown()
{
	GPIO_KEY=0x0f;//按键初始化
	if(GPIO_KEY!=0x0f)//读取按键是否按下
	{
		delay_10us(1000);//延时10ms进行消抖
		if(GPIO_KEY!=0x0f)//再次检测键盘是否按下
		{	
			//测试列
			GPIO_KEY=0X0F;
			switch(GPIO_KEY)
			{
				case(0X07):	KeyValue=0;break;//按键键值0-3，一列有四个按键
				case(0X0b):	KeyValue=1;break;
				case(0X0d): KeyValue=2;break;
				case(0X0e):	KeyValue=3;break;
			}
			//测试行
			GPIO_KEY=0XF0;
			switch(GPIO_KEY)
			{
				case(0X70):	KeyValue=KeyValue;break;//键值依次增加0、4、8、12
				case(0Xb0):	KeyValue=KeyValue+4;break;
				case(0Xd0): KeyValue=KeyValue+8;break;
				case(0Xe0):	KeyValue=KeyValue+12;break;
				/*
					假如获取的是第三行四列的键值
					行是第三行即0xd0，键值为8
					列是第四列即0xe0,键值为3
					8+3=12，因为从零开始所以还要减去1，道理和数组类似
					即三行四列的键值为11
				*/
			}
			
		}
	}
	Motor=0;//关闭风扇
	time0_init();//定时中断初始化	   
	 
	if(KeyValue==3){//功能1切换功能1-3	
		flag_lis1=1;//打开S5、S6的按键功能
		if(cd==1)Speed=20;//根据功能绑定PWM占比
		if(cd==2)Speed=30;
		if(cd==3)Speed=70;
		list(1);
	}
	if(KeyValue==4){//功能2 增加60s	倒计时
		if(flag_lis1)list(2);//flag_lis1初始不能用，要按下S4才能生效	   
	}
	if(KeyValue==5){//功能3  清零倒计时
		if(flag_lis1)list(3);//flag_lis1初始不能用，要按下S4才能生效
	}
	if(KeyValue==6){//功能4	 室温
		list(4);
	} 
	//检测按键松手检测
	if(GPIO_KEY==0xf0&&KeyValue==3)	
	{				
		if(list1_falg)cd+=1;//功能数字显示切换
		
		list1_falg=1;//打开增加功能切换
		if(cd>3)cd=1;//功能只有1-3	
	}
	//松手时增加或归零
	if(GPIO_KEY==0xf0&&KeyValue==4)
	{
		if(flag_lis1)count+=60;
		/*
			增加60s 倒计时	只有按了S4打开开关后生效 
			，按了后可以直接按归零或增加  按到室温时再关闭
		*/
	}
	if(GPIO_KEY==0xf0&&KeyValue==5)
	{
		if(flag_lis1)count=0;//清零	 倒计时	 只有按了S4打开开关后生效
	}
	if(GPIO_KEY==0xf0&&KeyValue==6)	
	{
	  	flag_lis1=0;//关闭S5-S6按键
		list1_falg=0;//关闭cd即功能切换
	}
}