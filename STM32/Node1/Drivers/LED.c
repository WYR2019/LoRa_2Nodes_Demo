#include "stm32f10x.h"                  // Device header
void LED_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;//PA0和PA1
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_SetBits(GPIOA,GPIO_Pin_0|GPIO_Pin_1);
}

//如果led较多，可以定义一个LED_Set函数，然后定义两个参数控制，一个参数选择操作哪个灯，一个参数选择开还是关

void LED1_ON(void)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_0);
}

void LED1_OFF(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_0);
}

void LED1_Turn(void)//实现按一下熄灭，再按一下点亮该如何实现，就是按键按下，LED的状态取反，实现电平翻转，一般用于输出模式下
{
	if(GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_0) == 0)//如果PA0的输出寄存器为0
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_0);//将PA0置1
	}else
	{
		GPIO_ResetBits(GPIOA,GPIO_Pin_0);
	}
}

void LED2_ON(void)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_1);
}

void LED2_OFF(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_1);
}

void LED2_Turn(void)//实现按一下熄灭，再按一下点亮该如何实现，就是按键按下，LED的状态取反，实现电平翻转，一般用于输出模式下
{
	if(GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_1) == 0)//如果PA0的输出寄存器为0
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_1);//将PA0置1
	}else
	{
		GPIO_ResetBits(GPIOA,GPIO_Pin_1);
	}
}
