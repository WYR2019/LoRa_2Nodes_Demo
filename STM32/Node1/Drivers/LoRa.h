#ifndef __LORA_H__
#define __LORA_H__
#include "stm32f10x.h"                                                   // Device header
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

/* LoRa的设置 */
#define LORA_MODE_INIT                                        1

#define LORA_GPIO_PIN_TX                                      GPIO_Pin_10
#define LORA_GPIO_PIN_RX                                      GPIO_Pin_11
#define LORA_GPIO_PIN_ATK_AUX                                 GPIO_Pin_4 //PA4
#define LORA_GPIO_PIN_ATK_MD0                                 GPIO_Pin_3 //PB3
#define LORA_GATE_ADDR_HIGH                                   0x03
#define LORA_GATE_ADDR_LOW                                    0xE9
#define LORA_GATE_CHANNEL                                     0x17
#define LORA_NODE_IDENTIFIER                                  0xD1

/* LoRa发送数据数组 */
void LoRa_USART3_Mode_Init(void);
void LoRa_USART3_SendByte(uint8_t byte);

#if LORA_MODE_INIT
extern uint8_t loRaSensorDHT11Identifier[];
extern uint8_t loRaExecutorLED[];
extern uint8_t loRaExecutorFan[];
extern uint8_t loRaExecutorHumidifier[];
extern uint8_t loRaExecutorStatusOn[];
extern uint8_t loRaExecutorStatusOff[];
extern uint8_t loRaUSART3RxPacket[];
extern uint8_t loRaUSART3RxFlag;
extern uint8_t executorState;
extern uint8_t executorID;

void LoRa_USART3_SendArray(uint8_t *array,uint16_t length);

void LoRa_USART3_Gate_IdPkt(void);
void LoRa_USART3_Node1_Send_Msg(void);

#else
void LoRa_USART3_SendString(char *string);
void LoRa_USART3_Printf(char *format, ...);
#endif
#endif
