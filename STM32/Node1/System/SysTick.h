#ifndef _SysTick_H
#define _SysTick_H

#include "stm32f10x.h"                  // Device header

void SysTick_Init(uint8_t SYSCLK);
void delay_ms(uint16_t nms);
void delay_us(uint32_t nus);



#endif
