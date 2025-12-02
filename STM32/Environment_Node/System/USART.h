#ifndef __USART_H__
#define __USART_H__

#define USE_SYSTEM                                              FREERTOS
#define NONE                                                    0
#define FREERTOS                                                1
#define OSAL                                                    2
#define vUsart1IrqHandler                                       USART1_IRQHandler
#define vEsp8266IrqHandler                                      USART2_IRQHandler
#define vLoRaIrqHandler                                         USART3_IRQHandler
#define USART1_GPIO_CLOCK                                       RCC_APB2Periph_GPIOA
#define USART2_GPIO_CLOCK                                       RCC_APB2Periph_GPIOA
#define USART3_GPIO_CLOCK                                       RCC_APB2Periph_GPIOB
#define USART1_GPIO_PORT                                        GPIOA
#define USART2_GPIO_PORT                                        GPIOA
#define USART3_GPIO_PORT                                        GPIOB
#define USART1_GPIO_PIN_TX                                      GPIO_Pin_9
#define USART1_GPIO_PIN_RX                                      GPIO_Pin_10
#define USART2_GPIO_PIN_TX                                      GPIO_Pin_2
#define USART2_GPIO_PIN_RX                                      GPIO_Pin_3
#define USART3_GPIO_PIN_TX                                      GPIO_Pin_10
#define USART3_GPIO_PIN_RX                                      GPIO_Pin_11

#if (USE_SYSTEM == NONE)
#elif (USE_SYSTEM == FREERTOS)
    #include "FreeRTOS.h"
    #include "queue.h"
#elif (USE_SYSTEM == OSAL)
#endif

#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#if (USE_SYSTEM == NONE)
#elif (USE_SYSTEM == FREERTOS)
    extern QueueHandle_t xQueueUsart1IrqHdlr;
    extern QueueHandle_t xQueueUsart2IrqHdlr;
    extern QueueHandle_t xQueueUsart3IrqHdlr;
#elif (USE_SYSTEM == OSAL)
#endif

void vUsartInit(USART_TypeDef *xUsartId, uint32_t ulBaudrate);
void vUsartSendByte(USART_TypeDef *xUsartId, uint8_t ucByte);
void vUsartSendArray(USART_TypeDef *xUsartId, uint8_t *pucArray, uint16_t usLength);
void vUsartSendString(USART_TypeDef *xUsartId, char *pcString);
void vUsartPrintf(USART_TypeDef *xUsartId, char *format, ...);

#endif
