#ifndef __USART3_H__
#define __USART3_H__

#include "stm32f10x.h"                  // Device header
#include "FreeRTOS.h"
#include "queue.h"
// #include "semphr.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define USART3_GPIO_PIN_TX                                      GPIO_Pin_10
#define USART3_GPIO_PIN_RX                                      GPIO_Pin_11
#define USART3_GPIO_PIN_ATK_AUX                                 GPIO_Pin_4 // PA4
#define USART3_GPIO_PIN_ATK_MD0                                 GPIO_Pin_3 // PB3
#define USART3_QUEUE_LENGTH                                     256

typedef struct 
{
    /* data */
    uint8_t *ucRxBuffer;
    uint8_t ucIndex;
}Usart3RecBuffer_t;

void vUsart3Init(uint32_t ulUsart3Baudrate);
void vUsart3SendByte(uint8_t ucByte);
void vUsart3SendArray(uint8_t *pucArray, uint16_t usLength);
void vUsart3SendString(char *pcString);
void vUsart3Printf(char *format, ...);

#endif
