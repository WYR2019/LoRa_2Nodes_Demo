#ifndef __ESP8266_H__
#define __ESP8266_H__

#include "stm32f10x.h"                                                          			// Device header
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "SysTick.h"
#include "USART1.h"
#include "LoRa_Gateway.h"

/*个人设置部分*/
#define ESP8266_GPIO_PIN_TX		        			GPIO_Pin_2
#define ESP8266_GPIO_PIN_RX		        			GPIO_Pin_3
      
#define WIFINAME  			          					"Exploration"                             //ssid
#define WIFIPASSWORD 		          					"IPSK25em."                               //password    
#define MQTTUSERNAME                  			"stm32"
#define MQTTPASSWORD                  			"IPSK25em."
#define MQTTCLIENTID                  			"EMQXSmartBadmintonGym&stm32"
#define MQTTBROKERIP                  			"121.36.104.9"
#define SUBSCRIBETOPIC                			"EMQXSmartBadmintonGym/devices/Android/topics"
#define PUBLISHTOPIC                  			"EMQXSmartBadmintonGym/devices/stm32/topics"

#define USART2_RX_MAX_SIZE  		          	2000  	                              			//定义最大接收字节数 300
#define ESP8266_USART2_MAX_RECV_LEN   			1024
#define ESP8266_USART2_MAX_SEND_LEN   			1024
#define BUFFER_SIZE         								500     																	//buff的size
#define ID_TEMP  														"temp"
#define ID_HUMI  														"humi"
#define ID_FAN															"fan"
#define ID_HUMIDIFIER												"humidifier"
#define ID_SMOKE														"smoke"
#define ID_FIRE															"fire"
#define ID_BEEP															"beep"
#define ID_SERVO														"servo"

void ESP8266_USART2_Init(void);
void ESP8266_Printf(char* format,...);
void ESP8266_Connection(void);
void ESP8266_Connection_Status(void);
void ESP8266_USART1_Scan_Actions(void);
void ESP8266_MQTT_Publish(void);
void ESP8266_Publish_Topics_Actions(void);
void ESP8266_Subscribe_Topics_Actions(void);

extern int size;
extern int sendData;//计算发布代码的次数
extern int pubLedSwitch, pubFanSwitch, pubHumidifierSwitch;

#endif
