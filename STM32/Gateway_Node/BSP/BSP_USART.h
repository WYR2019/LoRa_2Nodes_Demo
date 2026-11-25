#ifndef __USART_H__
#define __USART_H__

/* 是否使用FREERTOS。 */
#define ENABLE_FREERTOS                                                         1

#define USART1_GPIO_CLOCK                                                       RCC_APB2Periph_GPIOA
#define USART2_GPIO_CLOCK                                                       RCC_APB2Periph_GPIOA
#define USART3_GPIO_CLOCK                                                       RCC_APB2Periph_GPIOB
#define USART1_GPIO_PORT                                                        GPIOA
#define USART2_GPIO_PORT                                                        GPIOA
#define USART3_GPIO_PORT                                                        GPIOB
#define USART1_GPIO_PIN_TX                                                      GPIO_Pin_9
#define USART1_GPIO_PIN_RX                                                      GPIO_Pin_10
#define USART2_GPIO_PIN_TX                                                      GPIO_Pin_2
#define USART2_GPIO_PIN_RX                                                      GPIO_Pin_3
#define USART3_GPIO_PIN_TX                                                      GPIO_Pin_10
#define USART3_GPIO_PIN_RX                                                      GPIO_Pin_11

#if ENABLE_FREERTOS
    #include "FreeRTOS.h"
    #include "queue.h"
    #define ENABLE_FREERTOS_API_QUEUE_USART1_IRQ                                0
    #define ENABLE_FREERTOS_API_QUEUE_USART2_IRQ                                0
    #define ENABLE_FREERTOS_API_QUEUE_USART3_IRQ                                0
#else

#endif

#include "stm32f10x.h"                                                                                  // Device header
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#if ENABLE_FREERTOS_API_QUEUE_USART1_IRQ
    extern QueueHandle_t queueUsart1IrqHdlr;
#endif
#if ENABLE_FREERTOS_API_QUEUE_USART2_IRQ
    extern QueueHandle_t queueUsart2IrqHdlr;
#endif
#if ENABLE_FREERTOS_API_QUEUE_USART3_IRQ
    extern QueueHandle_t queueUsart3IrqHdlr;
#endif

#define usart1IrqHandler(void)                                                  USART1_IRQHandler(void)
#define trace(format, ...)                                                      usartPrintf(USART3, format, ##__VA_ARGS__)
#define traceFromIrq(rxData)                                                    USART_SendData(USART3, rxData);

void usartInit(USART_TypeDef *usartId, uint32_t baudrate);
void usartSendByte(USART_TypeDef *usartId, uint8_t byte);
void usartSendArray(USART_TypeDef *usartId, uint8_t *array, uint16_t length);
void usartSendString(USART_TypeDef *usartId, uint8_t *string);
void usartPrintf(USART_TypeDef *usartId, char *format, ...);

#endif
