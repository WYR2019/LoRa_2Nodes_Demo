//���һ����ֹͷ�ļ��ظ������Ĵ���if not define�����û�ж�����ַ�������ô���ȶ���һ��
#ifndef __LED_H__
#define __LED_H__

#include "stm32f10x.h"                  // Device header

#define LED_PC13_PORT         GPIOC
#define LED_PC13_GPIO_PIN     GPIO_Pin_13
#define LED_PC13_GPIO_CLOCK   RCC_APB2Periph_GPIOC

//��������
void LED_PC13_Init(void);
void LED_PC13_ON(void);
void LED_PC13_OFF(void);
void LED_PC13_Turn(void);

#endif
