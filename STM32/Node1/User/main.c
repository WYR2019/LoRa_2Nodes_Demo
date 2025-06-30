#include "stm32f10x.h"                  // Device header
#include "SysTick.h"
#include "OLED.h"
#include "LoRa.h"
#include "DHT11.h"
#include "LED_PC13.h"
#include "Relay.h"

uint8_t temp,humi;

int main(void){
  SysTick_Init(72);
  LoRa_USART3_Mode_Init();
  LED_PC13_Init();
  RELAY_Init();
	
  while(1)
  {
    #if LORA_MODE_INIT
    DHT11_Read_Data(&temp,&humi);
    LoRa_USART3_Node1_Send_Msg();
    if(executorState == 11)
    {
      LED_PC13_ON();
      LoRa_USART3_SendArray(loRaExecutorLED,1);
      LoRa_USART3_SendArray(loRaExecutorStatusOn,1);
    }
    else if(executorState == 12)
    {
      LED_PC13_OFF();
      LoRa_USART3_SendArray(loRaExecutorLED,1);
      LoRa_USART3_SendArray(loRaExecutorStatusOff,1);
    }
    else if(executorState == 21)
    {
      RELAY_ON;
      LoRa_USART3_SendArray(loRaExecutorFan,1);
      LoRa_USART3_SendArray(loRaExecutorStatusOn,1);
    }
    else if(executorState == 22)
    {
      RELAY_OFF;
      LoRa_USART3_SendArray(loRaExecutorFan,1);
      LoRa_USART3_SendArray(loRaExecutorStatusOff,1);
    }
    #endif
  }
}

#if LORA_MODE_INIT
void LoRa_USART3_Node1_Send_Msg(void)
{
  LoRa_USART3_Gate_IdPkt();
  LoRa_USART3_SendArray(loRaSensorDHT11Identifier,1);
  LoRa_USART3_SendArray(&temp,1);
  LoRa_USART3_SendArray(&humi,1);
}
#endif

/*bug��1����һ��ִ��ָ�����������ڶ��λ���һ���ӳ٣������βŻ���Ƴɹ������ҵڶ��νڵ㷢�������ɡ�0xEA����Ϊ��0xD1�������ѽ���� 
 *     2�������ط�����һ��ָ��󣬽ڵ㷢�������쳣�����ѽ���� 
 */
