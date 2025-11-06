#ifndef __CRC_H__
#define __CRC_H__

#include "stm32f10x.h"                  // Device header

void vCrcInit(void);
uint32_t vCalculateCrc32bit(uint32_t *pulData, uint32_t ulLength);

#endif
