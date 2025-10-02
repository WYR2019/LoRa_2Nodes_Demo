#ifndef _SysTick_H
#define _SysTick_H

#include "stm32f10x.h"                  // Device header

typedef struct {
  uint8_t ucFacUs;
  int16_t usFacMs;
} UcFacInit_t;

static UcFacInit_t xUcFacInit = {
  .ucFacUs = 0,
  .usFacMs = 0
};

void SysTick_Init(uint8_t ucSYSCLK);
void delay_ms(uint16_t usNms);
void delay_us(uint32_t ulNus);

#endif
