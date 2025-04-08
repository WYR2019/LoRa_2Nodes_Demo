#include "stm32f10x.h"                  // Device header
#include "Systick.h"
#include "OLED.h"
#include "LoRa.h"
#include "DHT11.h"

uint8_t temp;
uint8_t humi;

int main(void){
	SysTick_Init(72);
	LoRa_USART3_Trans_Mode_Init(9600);
	while(1)
	{
		DHT11_Read_Data(&temp,&humi);
		delay_ms(1500);
//		LoRa_USART3_Printf("temp %d ,humi %d\r\n",temp,humi);
		LoRa_USART3_IdentifierPkt();
		LoRa_USART3_SendArray(&temp,1);
		LoRa_USART3_SendArray(&humi,1);
	}
}
