#include "stm32f10x.h"                  // Device header
#include "SysTick.h"
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
		if(exeState == 1)
		{
			LED_PC13_ON();
		}
	}
}

void LoRa_USART3_Md_Trans_Msg(void)
{
	LoRa_USART3_Gate_IdentifierPkt();
	LoRa_USART3_SendArray(loraSensorDHT11Identifier,1);
	LoRa_USART3_SendArray(&temp,1);
	LoRa_USART3_SendArray(&humi,1);
}

//bug：无法正常关灯

