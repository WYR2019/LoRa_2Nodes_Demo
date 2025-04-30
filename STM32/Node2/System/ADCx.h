#ifndef _ADCX_H_
#define _ADCX_H_
#include "stm32f10x.h"                  // Device header

// ADC 编号选择
// 可以是 ADC1/2/3
#define    ADCx                          ADC1
#define    ADC_CLK                       RCC_APB2Periph_ADC1



void ADCx_Init(void);
uint16_t ADC_GetValue(uint8_t ADC_Channel,uint8_t ADC_SampleTime);

#endif
