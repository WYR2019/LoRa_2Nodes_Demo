#include "Relay.h"

/*****************辰哥单片机设计******************
											STM32
 * 文件			:	5V继电器c文件                   
 * 版本			: V1.0
 * 日期			: 2024.9.18
 * MCU			:	STM32F103C8T6
 * 接口			:	见代码							
 * BILIBILI	:	辰哥单片机设计
 * CSDN			:	辰哥单片机设计
 * 作者			:	辰哥

**********************BEGIN***********************/


void RELAY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RELAY_CLK, ENABLE ); //配置时钟
	
	GPIO_InitStructure.GPIO_Pin = RELAY_GPIO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(RELAY_GPIO_PROT,&GPIO_InitStructure);

	RELAY_OFF;
}
