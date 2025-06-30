#include "stm32f10x.h"                  // Device header
#include "USART1.h"
#include "SysTick.h"
#include "ESP8266.h"
#include "LoRa_Gateway.h"
#include "LED_PC13.h"
#include "OLED.h"

int main(void){
	SysTick_Init(72);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //中断优先级分组 分2组
	LoRa_USART3_Trans_Mode_Init(9600);
	USART1_Init();
	ESP8266_USART2_Init();
	OLED_Init();
//	MQ2_Init();
	
	delay_ms(1000);					//延迟
	
	ESP8266_Connection();
	
	while(1)
	{
		ESP8266_Publish_Topics_Actions();
		sendData++;
		ESP8266_MQTT_Publish();
		delay_ms(1);
		if(pubFanSwitch == 1)
		{
			LoRa_USART3_Node1_Fan_On_Msg();
		}
		else if(pubFanSwitch == 0)
		{
			LoRa_USART3_Node1_Fan_Off_Msg();
		}
		else if(pubHumidifierSwitch == 1)
		{
			LoRa_USART3_Node1_Humidifier_On_Msg();
		}
		else if(pubHumidifierSwitch == 0)
		{
			LoRa_USART3_Node1_Humidifier_Off_Msg();
		}
	}
}
