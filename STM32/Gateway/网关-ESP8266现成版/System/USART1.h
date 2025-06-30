#ifndef __USART1_H__
#define __USART1_H__

#include "stm32f10x.h"                  // Device header
#include "string.h"
#include <stdarg.h>
#include <stdio.h>
#include "ESP8266.h"

#define USART_REC_LEN  			300  	//�����������ֽ��� 200

void USART1_Init(void);
void Serial_SendByte(uint8_t Byte);

extern uint8_t  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern uint16_t USART_RX_STA;         				//����״̬���

#endif
