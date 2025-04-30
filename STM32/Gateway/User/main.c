#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "LoRa_Gateway.h"
#include "LED_PC13.h"
#include "ESP8266.h"

int main(void){
	SystemInit();
	delay_init(72);
	LoRa_USART3_Trans_Mode_Init(9600);
	ESP8266_USART2_Init(115200);
	ESP8266_USART2_Connection_Init();
	ESP8266_USART2_MQTT_Mode_Init();
	while(1)
	{
		ESP8266_USART2_Printf("Current temperature is:%d\r\n",loRaUSART3RxPacket[2]);
		delay_ms(1500);
		ESP8266_USART2_Printf("Current humidity is:%d\r\n",loRaUSART3RxPacket[3]);
//		LoRa_USART3_Node2_Cmd_Msg();
		delay_ms(1500);
	}
}
