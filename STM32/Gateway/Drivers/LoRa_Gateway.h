#ifndef __LORA_GATEWAY_H__
#define __LORA_GATEWAY_H__

#include "stm32f10x.h"                  															 // Device header
#include <stdio.h>
#include <stdarg.h>

/* LoRaµƒ…Ë÷√ */
#define LORA_GPIO_PIN_TX           													GPIO_Pin_10
#define LORA_GPIO_PIN_RX           													GPIO_Pin_11
#define LORA_GPIO_PIN_ATK_AUX      													GPIO_Pin_4 //PA4
#define LORA_GPIO_PIN_ATK_MD0			 													GPIO_Pin_3 //PB3

#define LORA_NODE1_ADDR_HIGH																0x03
#define LORA_NODE1_ADDR_LOW																	0xEA
#define LORA_NODE1_CHANNEL																	0x17
#define LORA_NODE1_IDENTIFIER																0xD1

#define LORA_NODE2_ADDR_HIGH																0x03
#define LORA_NODE2_ADDR_LOW																	0xEB
#define LORA_NODE2_CHANNEL																	0x17
#define LORA_NODE2_IDENTIFIER																0xD2

extern uint8_t loRaNode1Addr[2];
extern uint8_t loRaNode1Channel[1];
extern uint8_t loRaNode1Id[1];

extern uint8_t loRaNode2Addr[2];
extern uint8_t loRaNode2Channel[1];
extern uint8_t loRaNode2Id[1];

extern uint8_t loRaSensorDHT11Id[1];
extern uint8_t loRaSensorMQ2Id[1];
extern uint8_t loRaSensorLightId[1];
extern uint8_t loRaSensorFireId[1];
extern uint8_t loRaExecutorHumidifier[1];
extern uint8_t loRaExecutorFan[1];
extern uint8_t loRaExecutorBuzzer[1];
extern uint8_t loRaExecutorLED[1];
extern uint8_t loRaExecutorServo[1];
extern uint8_t loRaExecutorStepmotor[1];

extern uint8_t loRaExecutorStatusOn[1];
extern uint8_t loRaExecutorStatusOff[1];

void LoRa_USART3_Trans_Mode_Init(uint32_t mdTransBaudrate);
void LoRa_USART3_Set_Mode_Init(uint32_t mdSetBaudrate);
void LoRa_USART3_SendByte(uint8_t byte);
void LoRa_USART3_SendArray(uint8_t *array,uint16_t length);
void LoRa_USART3_SendString(char *string);
void LoRa_USART3_Printf(char *format, ...);
void LoRa_USART3_Node1IDPkt(void);
void LoRa_USART3_Node2IDPkt(void);

void LoRa_USART3_Node1_Cmd_Msg(void);
void LoRa_USART3_Node2_Cmd_Msg(void);

#endif
