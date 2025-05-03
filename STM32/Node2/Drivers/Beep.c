#include "Beep.h"

/*蜂鸣器程序*/
//先不要选A15、B3、B4这三个口连接蜂鸣器，这3个口默认作为JTAG的调试端口
void Beep_Init(void)
{
	RCC_APB2PeriphClockCmd(BEEP_GPIO_CLOCK,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = BEEP_GPIO_PIN;//PB5
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(BEEP_GPIO_PORT,&GPIO_InitStructure);
	GPIO_SetBits(BEEP_GPIO_PORT,BEEP_GPIO_PIN);
}

void Beep_ON(void)
{
	GPIO_ResetBits(BEEP_GPIO_PORT,BEEP_GPIO_PIN);
}

void Beep_OFF(void)
{
	GPIO_SetBits(BEEP_GPIO_PORT,BEEP_GPIO_PIN);
}

void Beep_Turn(void)//实现按一下熄灭，再按一下点亮该如何实现，就是按键按下，LED的状态取反，实现电平翻转，一般用于输出模式下
{
	if(GPIO_ReadOutputDataBit(BEEP_GPIO_PORT,BEEP_GPIO_PIN) == 0)//如果PA0的输出寄存器为0
	{
		GPIO_SetBits(BEEP_GPIO_PORT,BEEP_GPIO_PIN);//将PA0置1
	}else
	{
		GPIO_ResetBits(BEEP_GPIO_PORT,BEEP_GPIO_PIN);
	}
}
