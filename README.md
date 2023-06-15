# STC89C52_final
运用STC89C52单片机制作智能风扇控制系统
>>器件：矩阵按键、LED数码管、T0计数器、STC89C52单片机、电机等~
>>按键：矩阵按键S4-S7
>>S4 控制风扇的挡位，打开后默认60s倒计时，S5-S6的总开关
>>S4 增加风扇倒计时的时间（60s）
>>S5 清零风扇倒计时的时间
>>S6 显示数码管的室温

## 项目介绍
“模拟风扇控制系统”能够模拟电扇工作,通过按键控制风扇的转动速度与定时时间，数码管实时显示风扇的工作模式,动态倒计时剩余的定时时间
系统主要由数码管显示、单片机最小系统、按键输入和电机控制保护电路组成。
![image](https://github.com/lyidle/STC89C52_final/assets/51476712/c599a7ca-73f2-471a-b9ea-af8c80a05b62)

## 项目方案
本设计的温控风扇利用温度传感器DS18B20来检测外界环境的温度，利用数码管显示境温度和风度档位，既可以通过控制按键人工调节开启温度以及风速，也可实现风速的自动控制。
并可以将定时时间存入AT24C02芯片，实现数据的掉电保护。风扇共有十个档位，根据PWM来控制调节风扇速度。
![image](https://github.com/lyidle/STC89C52_final/assets/51476712/3907e9cc-4cb8-4369-ac4f-1b5467a07a25)
本系统采用DS18B20温度传感器测温，74HC573驱动数码管显示温度和风扇档位；4个按键：睡眠风、自然风、常风、室温。
“睡眠风”模式下，对应PWM占空比为20%；
“睡眠风”模式下，对应PWM占空比为20%；
“常风”模式下，对应PWM占空比为70%； 
“室温”按键，按下S7，通过数码管显示当前室温
![image](https://github.com/lyidle/STC89C52_final/assets/51476712/c54dbd2e-314a-4863-a6dc-e1c67737abc2)

## 开发环境
操作系统：Windows 11
代码编写：Keil uVision4
程序烧录：Stc-isp-15xx-v6.86rAltium Designer
原理图绘画：Altium Designer

# 软硬件设计
## 主控芯片选型
主控芯片我们采用51单片机（也称为8051单片机）是一种非常常见的8位单片机，最初由Intel公司于1981年推出。它具有以下几个主要特点：

8位体系结构：51单片机是一种8位的处理器，每次可以处理8位数据。
小型封装：51单片机通常采用DIP（双列直插式）封装，这种封装非常小巧，易于焊接到电路板上。
内置存储器：51单片机具有内置的ROM（只读存储器）和RAM（随机存储器）。
丰富的外设接口：51单片机具有丰富的外设接口，包括通用输入输出引脚（GPIO）、串行通信接口（如UART、SPI和I2C）、定时器/计数器、中断控制器等。

综上所述，基于51单片机的优点和项目实际，所以在本次实训中选用51单片机作为主控芯片。
![image](https://github.com/lyidle/STC89C52_final/assets/51476712/e6e3010f-bf81-4e08-b954-e231df10f8a7)

