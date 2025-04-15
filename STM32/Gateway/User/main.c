#include "stm32f10x.h"                  // Device header
#include "LoRa_Gateway.h"
#include "Delay.h"

int main(void){
	SystemInit();
	delay_init(72);
	LoRa_USART3_Trans_Mode_Init(9600);
	while(1)
	{
		delay_ms(1500);
		LoRa_USART3_Node1_Cmd_Msg();
		delay_ms(1500);
		LoRa_USART3_Node2_Cmd_Msg();
	}
}


