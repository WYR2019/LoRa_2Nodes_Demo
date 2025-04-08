#include "stm32f10x.h"                  // Device header
#include "Systick.h"
#include "OLED.h"
#include "LoRa.h"
#include "DHT11.h"
#include "LED_PC13.h"

uint8_t temp,humi;

int main(void){
	SysTick_Init(72);
	LoRa_USART3_Trans_Mode_Init(9600);
	LED_PC13_Init();
	while(1)
	{
		DHT11_Read_Data(&temp,&humi);
		delay_ms(1500);
		LoRa_USART3_Md_Trans_Msg();
		if(LoRa_USART3_GetRxFlag() == 1)
		{
			LED_PC13_ON();
			LoRa_USART3_SendArray(LoRa_Executor_LED,1);
			LoRa_USART3_SendArray(LoRa_LED_Status_On,1);
		}
		else if(LoRa_USART3_GetRxFlag() == 2)
		{
			LED_PC13_OFF();
			LoRa_USART3_SendArray(LoRa_Executor_LED,1);
			LoRa_USART3_SendArray(LoRa_LED_Status_Off,1);
		}
	}
}

void LoRa_USART3_Md_Trans_Msg(void)
{
	LoRa_USART3_IdentifierPkt();
	LoRa_USART3_SendArray(LoRa_Sensor_DHT11_Identifier,1);
	LoRa_USART3_SendArray(&temp,1);
	LoRa_USART3_SendArray(&humi,1);
}

//bug：无法正常关灯

