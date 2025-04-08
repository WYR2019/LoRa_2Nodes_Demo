#ifndef __LORA_GATEWAY_H__
#define __LORA_GATEWAY_H__

#include "stm32f10x.h"                  																		// Device header
#include <stdio.h>
#include <stdarg.h>

/* LoRaµƒ…Ë÷√ */
#define LORA_GPIO_PIN_TX           														GPIO_Pin_10
#define LORA_GPIO_PIN_RX           														GPIO_Pin_11
#define LORA_GPIO_PIN_ATK_AUX      														GPIO_Pin_4 	 	//PA4
#define LORA_GPIO_PIN_ATK_MD0			 														GPIO_Pin_3 		//PB3

void LoRa_USART3_Trans_Mode_Init(uint32_t Md_Trans_BaudRate);
void LoRa_USART3_Set_Mode_Init(uint32_t Md_Set_BaudRate);
void LoRa_USART3_SendByte(uint8_t Byte);
void LoRa_USART3_SendArray(uint8_t *Array,uint16_t Length);
void LoRa_USART3_SendString(char *String);
void LoRa_USART3_Printf(char *format, ...);
void LoRa_USART3_Node1IDsPkt(void);
void LoRa_USART3_Node2IDsPkt(void);

#endif
