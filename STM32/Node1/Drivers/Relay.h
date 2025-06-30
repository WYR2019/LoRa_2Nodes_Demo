#ifndef __RELAY_H
#define	__RELAY_H
#include "stm32f10x.h"
#include "SysTick.h"

/*****************辰哥单片机设计******************
											STM32
 * 文件			:	5V继电器h文件                   
 * 版本			: V1.0
 * 日期			: 2024.9.18
 * MCU			:	STM32F103C8T6
 * 接口			:	见代码							
 * BILIBILI	:	辰哥单片机设计
 * CSDN			:	辰哥单片机设计
 * 作者			:	辰哥

**********************BEGIN***********************/


/***************根据自己需求更改****************/
// 继电器 GPIO宏定义

#define	RELAY_CLK							RCC_APB2Periph_GPIOA

#define RELAY_GPIO_PIN 				GPIO_Pin_0

#define RELAY_GPIO_PORT 			GPIOA

#define RELAY_ON 							GPIO_SetBits(RELAY_GPIO_PORT,RELAY_GPIO_PIN)
#define RELAY_OFF 						GPIO_ResetBits(RELAY_GPIO_PORT,RELAY_GPIO_PIN)


/*********************END**********************/

void RELAY_Init(void);

#endif



