#ifndef __USART_H__
#define __USART_H__

/* 是否使用RTOS，若需要使用RTOS，则将下面对应的操作系统的预编译设置需要使用到的RTOS名称，反之则设置为NONE。 */
#define USE_RTOS                                                FREERTOS
#define NONE                                                    0
#define FREERTOS                                                1

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

#if (USE_RTOS == NONE)

#elif (USE_RTOS == FREERTOS)
    #include "FreeRTOS.h"
    #include "queue.h"
    #define ENABLE_FREERTOS_API_QUEUE_USART1_IRQ                0
    #define ENABLE_FREERTOS_API_QUEUE_USART2_IRQ                0
    #define ENABLE_FREERTOS_API_QUEUE_USART3_IRQ                1
#endif

#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#if (ENABLE_FREERTOS_API_QUEUE_USART1_IRQ == 1)
    extern QueueHandle_t xQueueUsart1IrqHdlr;
#endif
#if (ENABLE_FREERTOS_API_QUEUE_USART2_IRQ == 1)
    extern QueueHandle_t xQueueUsart2IrqHdlr;
#endif
#if (ENABLE_FREERTOS_API_QUEUE_USART3_IRQ == 1)
    extern QueueHandle_t xQueueUsart3IrqHdlr;
#endif

extern volatile uint8_t ucTcpClosedFlag;

#define                 BUFFER_MAX_LENGTH                      1024                                                                                                // 最大接收缓存字节数
extern struct SerialFrame_t                                                                                                                                // 串口数据帧的处理结构体
{
    char  cSerialReceivedBuffer [ BUFFER_MAX_LENGTH ];
    union {
    __IO uint16_t usInfoAll;
    struct {
            __IO uint16_t usFrameLength       :15;                                                                                                                 // 14:0
            __IO uint16_t usFrameFinishFlag   :1;                                                                                                                  // 15
        } Bits_t;
    }; 
} xSerialFrameRecord;

void vUsartInit(USART_TypeDef *xUsartId, uint32_t ulBaudrate);
void vUsartSendByte(USART_TypeDef *xUsartId, uint8_t ucByte);
void vUsartSendArray(USART_TypeDef *xUsartId, uint8_t *pucArray, uint16_t usLength);
void vUsartSendString(USART_TypeDef *xUsartId, char *pcString);
void vUsartPrintf(USART_TypeDef *xUsartId, char *format, ...);

#endif
