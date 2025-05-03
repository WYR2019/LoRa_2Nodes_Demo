#ifndef __SERVO_H
#define	__SERVO_H
#include "stm32f10x.h"
#include "SysTick.h"

/*****************辰哥单片机设计******************
											STM32
 * 文件			:	舵机驱动驱动h文件                   
 * 版本			: V1.0
 * 日期			: 2024.9.27
 * MCU			:	STM32F103C8T6
 * 接口			:	见代码							
 * BILIBILI	:	辰哥单片机设计
 * CSDN			:	辰哥单片机设计
 * 作者			:	辰哥

**********************BEGIN***********************/


/***************根据自己需求更改****************/
// SERVO舵机 GPIO宏定义

#define	SERVO_CLK										RCC_APB2Periph_GPIOA

#define SERVO_GPIO_PIN 							GPIO_Pin_2
#define SERVO_GPIO_PORT  						GPIOA


/*********************END**********************/

void Servo_Init(void);
void PWM_SetCompare3(uint16_t Compare);
void Servo_SetAngle(float Angle);

#endif



