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
		if (nodeID == 1)
		{
			LED_PC13_ON();
		}
		else if (nodeID == 2)
		{
			LED_PC13_OFF();
		}
	}
}
