#ifndef __BEEP_H__
#define __BEEP_H__

#include "stm32f10x.h"                  // Device header

#define BEEP_GPIO_PORT  GPIOB
#define BEEP_GPIO_PIN   GPIO_Pin_5
#define BEEP_GPIO_CLOCK RCC_APB2Periph_GPIOB

void Beep_Init(void);
void Beep_ON(void);
void Beep_OFF(void);
void Beep_Turn(void);

#endif
