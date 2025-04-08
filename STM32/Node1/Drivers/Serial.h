#ifndef __Serial_H__
#define __Serial_H__
#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>

//数组声明外部可调用
extern uint8_t Serial_TxPacket[];
extern uint8_t Serial_RxPacket[];

void Serial_Init(void);
void Serial_SendByte(uint8_t Byte);
void Serial_SendArray(uint8_t *Array,uint16_t Length);
void Serial_SendString(char *String);
void Serial_SendNumber(uint32_t Number,uint8_t Length);
void Serial_Printf(char *format, ...);

void Serial_SendPacket(void);

uint8_t Serial_GetRxFlag(void);
//uint8_t Serial_GetRxData(void);

#endif
