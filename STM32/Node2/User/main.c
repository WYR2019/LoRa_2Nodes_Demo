#include "stm32f10x.h"                  // Device header
#include "Systick.h"
#include "OLED.h"
#include "LoRa.h"
#include "MQ2.h"
#include "ADCx.h"

uint16_t value;
uint8_t buff[30];//参数显示缓存数组
float ppm;
int main(void){
	SysTick_Init(72);
	LoRa_USART3_Trans_Mode_Init(9600);
	MQ2_Init();

	while(1)
	{
		value = MQ2_GetData();
//		delay_ms(1500);
//		LoRa_USART3_IdentifierPkt();
//		delay_ms(1500);
		LoRa_USART3_Printf("%d\r\n",value);
//		LoRa_USART3_SendArray(&value,2);
		delay_ms(1500);
		ppm = MQ2_GetData_PPM();
		LoRa_USART3_Printf((char*)buff, "%.2fppm    ",ppm);
	}
}
