#ifndef __USART1_H__
#define __USART1_H__

#include "stm32f10x.h"                  // Device header
#include "string.h"
#include <stdarg.h>
#include <stdio.h>
#include "ESP8266.h"

#define USART_REC_LEN  			300  	//定义最大接收字节数 200

void USART1_Init(void);
void Serial_SendByte(uint8_t Byte);

extern uint8_t  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern uint16_t USART_RX_STA;         				//接收状态标记

#endif
