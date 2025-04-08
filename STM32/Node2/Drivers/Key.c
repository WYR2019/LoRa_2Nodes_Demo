#include "stm32f10x.h"                  // Device header
#include "Delay.h"
/*按键*/
//两个按键接在PB0和PB10上,按键采用上拉输入方式使能
void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_10;//PB0和PB10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//在输入模式中，这个参数没有用处
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}

uint8_t Key_GetNum(void)//调用这个函数，就可以返回按下键的键码 uint8_t就是nusigned char的意思
{
	uint8_t KeyNum = 0;
	/*
		读取GPIO端口的功能，找到GPIO的读取函数，uint8_t和uint16_t开头的
		GPIO_ReadInputDataBit()读取输入数据寄存器某一个端口的输入值，返回值是uint8_t，代表着这个端口的高低电平，读取按键就需要用到这个函数
		GPIO_ReadInputData()读取整个输入寄存器的，返回值是uint16_t，是一个16位数据，每一位代表一个端口值
		GPIO_ReadOutputDataBit()读取输出寄存器某一个端口的值
		GPIO_ReadOutputData()读取整个输出寄存器的
	*/
	//PB0按键检测
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0) == 0)//如果读取PB1端口为0，也就是当按键按下后
	{
		Delay_ms(20);//去除按键抖动
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0) == 0);//检测按键松手的情况,如果按键没有松手，就持续卡在这里，直到松手
		Delay_ms(20);
		KeyNum = 1;//赋值，将键码=1传递出去
	}
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10) == 0)//如果读取PB1端口为0，也就是当按键按下后
	{
		Delay_ms(20);//去除按键抖动
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10) == 0);//检测按键松手的情况,如果按键没有松手，就持续卡在这里，直到松手
		Delay_ms(20);
		KeyNum = 2;//赋值，将键码=1传递出去
	}
	return KeyNum;//按键默认给0，如果没有按键按下，就返回0
}
