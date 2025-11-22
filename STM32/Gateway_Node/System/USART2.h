#ifndef __USART2_H__
#define __USART2_H__

#define USE_NONE_SYSTEM                                         0
#define USE_FREERTOS                                            1
#define USE_OSAL                                                0
#define vEsp8266IrqHandler                                      USART2_IRQHandler
#define USART2_GPIO_PIN_TX                                      GPIO_Pin_2
#define USART2_GPIO_PIN_RX                                      GPIO_Pin_3

#if (USE_NONE_SYSTEM == 1)
#elif (USE_FREERTOS == 1)
    #include "FreeRTOS.h"
    #include "queue.h"
#elif (USE_OSAL == 1)
#endif

#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#if (USE_NONE_SYSTEM == 1)
#elif (USE_FREERTOS == 1)
    extern QueueHandle_t xQueueUsart2IrqHdlr;
#elif (USE_OSAL == 1)
#endif

void vUsart2Init(uint32_t ulUsart2Baudrate);
void vUsart2SendByte(uint8_t ucByte);
void vUsart2SendArray(uint8_t *pucArray, uint16_t usLength);
void vUsart2SendString(char *pcString);
void vUsart2Printf(char *format, ...);

#endif
