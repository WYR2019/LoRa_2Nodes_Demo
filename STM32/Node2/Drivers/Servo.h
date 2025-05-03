#ifndef __SERVO_H
#define	__SERVO_H
#include "stm32f10x.h"
#include "SysTick.h"

/*****************���絥Ƭ�����******************
											STM32
 * �ļ�			:	�����������h�ļ�                   
 * �汾			: V1.0
 * ����			: 2024.9.27
 * MCU			:	STM32F103C8T6
 * �ӿ�			:	������							
 * BILIBILI	:	���絥Ƭ�����
 * CSDN			:	���絥Ƭ�����
 * ����			:	����

**********************BEGIN***********************/


/***************�����Լ��������****************/
// SERVO��� GPIO�궨��

#define	SERVO_CLK										RCC_APB2Periph_GPIOA

#define SERVO_GPIO_PIN 							GPIO_Pin_2
#define SERVO_GPIO_PORT  						GPIOA


/*********************END**********************/

void Servo_Init(void);
void PWM_SetCompare3(uint16_t Compare);
void Servo_SetAngle(float Angle);

#endif



