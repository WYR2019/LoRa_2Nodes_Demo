#ifndef __SysTick_H__
#define __SysTick_H__

#include "stm32f10x.h"                  // Device header

#define SYSTEM                          SYSTEM_FREERTOS
#define SYSTEM_NONE                     0
#define SYSTEM_FREERTOS                 1
#define SYSTEM_UCOSII                   2
#define SYSTEM_UCOSIII                  3
#define SYSTEM_RT_THREAD                4
#define SYSTEM_AZURE_RTOS               5

typedef struct {
  uint8_t ucFacUs;
  int16_t usFacMs;
} UcFacInit_t;

static UcFacInit_t xUcFacInit = {
  .ucFacUs = 0,
  .usFacMs = 0
};

void vSysTickInit(uint8_t ucSYSCLK);
void vDelayUs(uint32_t ulNus);
void vDelayMs(uint16_t usNms);

#endif
