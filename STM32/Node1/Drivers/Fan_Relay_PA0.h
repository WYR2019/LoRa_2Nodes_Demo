#ifndef __RELAY_H__
#define	__RELAY_H__
#include "stm32f10x.h"

#define	RELAY_CLK             RCC_APB2Periph_GPIOA
#define RELAY_GPIO_PIN        GPIO_Pin_0
#define RELAY_GPIO_PORT       GPIOA

void Fan_Relay_Init(void);
void Fan_Relay_On(void);
void Fan_Relay_Off(void);


#endif
