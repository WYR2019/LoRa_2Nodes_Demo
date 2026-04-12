#ifndef __USART_H__
#define __USART_H__

/* 是否使用FREERTOS。 */
#define ENABLE_FREERTOS                                         1

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

#if ENABLE_FREERTOS
    #include "FreeRTOS.h"
    #include "queue.h"
    #define ENABLE_FREERTOS_API_QUEUE_USART1_IRQ                0
    #define ENABLE_FREERTOS_API_QUEUE_USART2_IRQ                0
    #define ENABLE_FREERTOS_API_QUEUE_USART3_IRQ                1
#else

#endif

#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#if ENABLE_FREERTOS_API_QUEUE_USART1_IRQ
    extern QueueHandle_t xQueueUsart1IrqHdlr;
#endif
#if ENABLE_FREERTOS_API_QUEUE_USART2_IRQ
    extern QueueHandle_t xQueueUsart2IrqHdlr;
#endif
#if ENABLE_FREERTOS_API_QUEUE_USART3_IRQ
    extern QueueHandle_t xQueueUsart3IrqHdlr;
#endif

#define vUsart1IrqHandler(void)                                 USART1_IRQHandler(void)
#define vEsp8266IrqHandler(void)                                USART2_IRQHandler(void)
#define vLoRaIrqHandler(void)                                   USART3_IRQHandler(void)
#define vDebugLogPrint(format, ...)                             vUsartPrintf(USART1, format, ##__VA_ARGS__)

#define BUFFER_MAX_LENGTH                                       512                         // 接收缓冲区最大字节数
typedef struct                                                                              // 串口数据帧的处理结构体
{
    char cRecvBuffer[BUFFER_MAX_LENGTH];
    union
    {
        /* data */
        volatile uint16_t usRecvDataLength                      :15;                        // 14:0
        volatile uint16_t usRecvFinishFlag                      :1;                         // 15
    }Flags_t;
}SerialReceiver_t;

void vUsartInit(USART_TypeDef *xUsartId, uint32_t ulBaudrate);
void vUsartSendByte(USART_TypeDef *xUsartId, uint8_t ucByte);
void vUsartSendArray(USART_TypeDef *xUsartId, uint8_t *pucArray, uint16_t usLength);
void vUsartSendString(USART_TypeDef *xUsartId, char *pcString);
void vUsartPrintf(USART_TypeDef *xUsartId, char *format, ...);

#endif
