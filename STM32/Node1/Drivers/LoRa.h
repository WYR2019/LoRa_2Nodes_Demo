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
#define LORA_GATE_ADDR_HIGH																		0x03
#define LORA_GATE_ADDR_LOW																		0xE9
#define LORA_GATE_CHANNEL																			0x17
#define LORA_NODE_IDENTIFIER																	0xD1

/* LoRa发送数据数组 */
extern uint8_t LoRa_USART3_RxPacket[];
extern uint8_t LoRa_Sensor_DHT11_Identifier[];
extern uint8_t LoRa_Executor_LED[];
extern uint8_t LoRa_LED_Status_On[];
extern uint8_t LoRa_LED_Status_Off[];
extern uint8_t LoRa_USART3_RXFlag;

void LoRa_USART3_Trans_Mode_Init(uint32_t Md_Trans_BaudRate);
void LoRa_USART3_Set_Mode_Init(uint32_t Md_Set_BaudRate);
void LoRa_USART3_SendByte(uint8_t Byte);
void LoRa_USART3_SendArray(uint8_t *Array,uint16_t Length);
void LoRa_USART3_SendString(char *String);
void LoRa_USART3_Printf(char *format, ...);
void LoRa_USART3_IdentifierPkt(void);
void LoRa_USART3_Md_Trans_Msg(void);
uint8_t LoRa_USART3_GetRxFlag(void);

#endif
