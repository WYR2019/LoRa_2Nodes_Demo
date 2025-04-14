#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "LoRa.h"
#include "DHT11.h"
#include "LED_PC13.h"

uint8_t temp,humi;

int main(void){
	SystemInit();
	delay_init(72);
	LoRa_USART3_Trans_Mode_Init(9600);
	LED_PC13_Init();
	while(1)
	{
		DHT11_Read_Data(&temp,&humi);
		delay_ms(1500);
		LoRa_USART3_Md_Trans_Msg();
		if(LoRa_USART3_GetRxFlag() == 31)
		{
			LED_PC13_ON();
			LoRa_USART3_SendArray(loraExecutorLED,1);
			LoRa_USART3_SendArray(loraLEDStatusOn,1);
		}
		else if(LoRa_USART3_GetRxFlag() == 30)
		{
			LED_PC13_OFF();
			LoRa_USART3_SendArray(loraExecutorLED,1);
			LoRa_USART3_SendArray(loraLEDStatusOff,1);
		}
	}
}

void LoRa_USART3_Md_Trans_Msg(void)
{
	LoRa_USART3_IdentifierPkt();
	LoRa_USART3_SendArray(loraSensorDHT11Identifier,1);
	LoRa_USART3_SendArray(&temp,1);
	LoRa_USART3_SendArray(&humi,1);
}

//bug：无法正常关灯

