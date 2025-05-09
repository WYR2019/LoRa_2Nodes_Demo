#ifndef __ESP8266_H__
#define __ESP8266_H__

#include "stm32f10x.h"                  																								// Device header
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "SysTick.h"
#include "LED_PC13.h"

#define ESP8266_GPIO_PIN_TX     						 	GPIO_Pin_2
#define ESP8266_GPIO_PIN_RX									 	GPIO_Pin_3
#define WIFINAME														 	"Exploration"
#define	WIFIPASSWORD												 	"IPSK25em."

/* MQTT模式下的个人设置 */
#define MQTTUSERNAME                  			 	"stm32"
#define MQTTPASSWORD                  			 	"IPSK25em."
#define MQTTCLIENTID                  			 	"EMQXLoRa2NodesDemo&stm32"
#define MQTTBROKERIP                  			 	"36.111.167.90"
#define MQTTSUBSCRIBETOPIC                	 	"EMQXLoRa2NodesDemo/devices/Android/topics"
#define MQTTPUBLISHTOPIC                  	  "EMQXLoRa2NodesDemo/devices/stm32/topics"

extern char esp8266RxPacket[];
extern uint8_t esp8266ConFinishState;

void ESP8266_USART2_Init(uint32_t BaudRate);
void ESP8266_USART2_SendByte(uint8_t Byte);
void ESP8266_USART2_SendString(char *String);
void ESP8266_USART2_Printf(char *format, ...);
uint8_t ESP8266_GetRxFlag(void);
void ESP8266_USART2_Connection_Init(void);
uint8_t ESP8266_USART2_MQTT_Mode_Init(void);
//void ESP8266_USART2_MQTT_Mode_Publish(void);

#endif
