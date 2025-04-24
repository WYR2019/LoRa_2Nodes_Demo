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
		if(executorState == 11)
		{
			LED_PC13_ON();
//			LoRa_USART3_SendArray(loRaLEDStatusOn,1);
		}
		else if(executorState == 12)
		{
			LED_PC13_OFF();
//			LoRa_USART3_SendArray(loRaLEDStatusOff,1);
		}
	}
}

void LoRa_USART3_Md_Trans_Msg(void)
{
	LoRa_USART3_Gate_IdentifierPkt();
	LoRa_USART3_SendArray(loRaSensorDHT11Identifier,1);
	LoRa_USART3_SendArray(&temp,1);
	LoRa_USART3_SendArray(&humi,1);
}

/*bug：1、第一次执行指令正常，但第二次会有一次延迟，第三次才会控制成功，而且第二次节点发送数据由“0xEA”变为“0xD1”。（已解决） 
 *     2、在网关发布第一次指令后，节点发送数据异常。（已解决） 
 *
 */
