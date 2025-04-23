#ifndef __LORA_H__
#define __LORA_H__
#include "stm32f10x.h"                  																 		// Device header
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "LED_PC13.h"

/* LoRa的设置 */
#define LORA_GPIO_PIN_TX           														GPIO_Pin_10
#define LORA_GPIO_PIN_RX           														GPIO_Pin_11
#define LORA_GPIO_PIN_ATK_AUX      														GPIO_Pin_4 	 	//PA4
#define LORA_GPIO_PIN_ATK_MD0			 														GPIO_Pin_3 		//PB3
#define LORA_GATE_ADDR_HIGH																		0x03
#define LORA_GATE_ADDR_LOW																		0xE9
#define LORA_GATE_CHANNEL																			0x17
#define LORA_NODE_IDENTIFIER																	0xD1

/* LoRa发送数据数组 */
extern uint8_t loraSensorDHT11Identifier[];
extern uint8_t loraExecutorLED[];
extern uint8_t loraLEDStatusOn[];
extern uint8_t loraLEDStatusOff[];
extern uint8_t loraUsart3RxPacket[];
extern uint8_t loraUsart3RxFlag;
extern uint8_t exeState;

void LoRa_USART3_Trans_Mode_Init(uint32_t mdTransBaudrate);
void LoRa_USART3_Set_Mode_Init(uint32_t mdSetBaudrate);
void LoRa_USART3_SendByte(uint8_t byte);
void LoRa_USART3_SendArray(uint8_t *array,uint16_t length);
void LoRa_USART3_SendString(char *string);
void LoRa_USART3_Printf(char *format, ...);
void LoRa_USART3_Gate_IdentifierPkt(void);
void LoRa_USART3_Md_Trans_Msg(void);
//uint8_t LoRa_USART3_GetRxFlag(void);

#endif
