#ifndef __LORA_H__
#define __LORA_H__
#include "stm32f10x.h"                  																 		// Device header
#include <stdio.h>
#include <stdarg.h>

/* LoRa的设置 */
#define LORA_GPIO_PIN_TX           														GPIO_Pin_10
#define LORA_GPIO_PIN_RX           														GPIO_Pin_11
#define LORA_GPIO_PIN_ATK_AUX      														GPIO_Pin_4 	 	//PA4
#define LORA_GPIO_PIN_ATK_MD0			 														GPIO_Pin_3 		//PB3
#define LORA_DEST_ADDR_HIGH																		0x03
#define LORA_DEST_ADDR_LOW																		0xE9
#define LORA_DEST_CHANNEL																			0x17
#define LORA_NODE_IDENTIFIER																	0xD2

/* LoRa发送数据数组 */
extern uint8_t loRaSensorDHT11Identifier[];
extern uint8_t loRaExecutorLED[];
extern uint8_t loRaLEDStatusOn[];
extern uint8_t loRaLEDStatusOff[];
extern uint8_t loRaUSART3RxPacket[];
extern uint8_t loRaUSART3RxFlag;
extern uint8_t executorState;
extern uint8_t executorID;
extern uint8_t rxState;

void LoRa_USART3_Trans_Mode_Init(uint32_t Md_Trans_BaudRate);
void LoRa_USART3_SendByte(uint8_t Byte);
void LoRa_USART3_SendArray(uint16_t *Array,uint16_t Length);
void LoRa_USART3_SendString(char *String);
void LoRa_USART3_Printf(char *format, ...);
uint32_t Serial_Pow(uint32_t X, uint32_t Y);
void LoRa_USART3_SendNumber(uint32_t Number,uint8_t Length);
void LoRa_USART3_IdentifierPkt(void);
uint8_t LoRa_USART3_GetRxFlag(void);

#endif
