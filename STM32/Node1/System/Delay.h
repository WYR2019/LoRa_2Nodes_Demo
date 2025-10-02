#ifndef __DELAY_H__
#define __DELAY_H__

#include "stm32f10x.h"
#include "core_cm3.h"

void delay_us(uint32_t us);//微秒级别
void delay_ms(uint32_t ms);//毫秒级别

#endif
