#ifndef __RELAY_H
#define	__RELAY_H
#include "stm32f10x.h"
#include "SysTick.h"

/*****************���絥Ƭ�����******************
											STM32
 * �ļ�			:	5V�̵���h�ļ�                   
 * �汾			: V1.0
 * ����			: 2024.9.18
 * MCU			:	STM32F103C8T6
 * �ӿ�			:	������							
 * BILIBILI	:	���絥Ƭ�����
 * CSDN			:	���絥Ƭ�����
 * ����			:	����

**********************BEGIN***********************/


/***************�����Լ��������****************/
// �̵��� GPIO�궨��

#define	RELAY_CLK							RCC_APB2Periph_GPIOA

#define RELAY_GPIO_PIN 				GPIO_Pin_0

#define RELAY_GPIO_PORT 			GPIOA

#define RELAY_ON 							GPIO_SetBits(RELAY_GPIO_PORT,RELAY_GPIO_PIN)
#define RELAY_OFF 						GPIO_ResetBits(RELAY_GPIO_PORT,RELAY_GPIO_PIN)


/*********************END**********************/

void RELAY_Init(void);

#endif



