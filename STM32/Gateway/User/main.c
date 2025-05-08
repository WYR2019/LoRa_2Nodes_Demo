#include "stm32f10x.h"                  // Device header
#include "SysTick.h"
#include "LoRa_Gateway.h"
#include "LED_PC13.h"
#include "ESP8266.h"

int main(void){
	SysTick_Init(72);
	LoRa_USART3_Trans_Mode_Init(9600);
	ESP8266_USART2_Init(115200);
	ESP8266_USART2_Connection_Init();
	ESP8266_USART2_MQTT_Mode_Init();
	LED_PC13_Init();
	while(1)
	{
//		ESP8266_USART2_MQTT_Mode_Publish();
//		ESP8266_USART2_Printf("AT+MQTTPUB=0,\"%s\",\"{\"name\":\"LoRa2NodeDemo\", \"Node1:\"{\"Temperature:\"\"%d\",\"Humidity:\"\"%d\"}\",0,0\r\n",MQTTPUBLISHTOPIC,jsonTemp,jsonHumi);
//		delay_ms(1000);
	}
}
