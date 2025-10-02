#include "stm32f10x.h"                  // Device header
#include "SysTick.h"
#include "OLED.h"
#include "LoRa.h"
#include "DHT11.h"
#include "LED_PC13.h"
#include "Fan_Relay_PA0.h"

/**
  ******************************************************************************
  * @file    main.c
  * @author  github/WYR2019
  * @version V1.0
  * @date    2025/5/25
  * @brief   �������ļ�
  ******************************************************************************
  * @attention
  * 1������ĿΪ����STM32��LoRa�����ڻ������ϵͳ���������ǽڵ�1�Ľڵ�����롣
	* 2��LoRa���ݲɼ��ڵ�1��Ҫʹ��DHT11�������ɼ���ʪ�ȣ��û������ƶ���Զ�̿��Ʒ��ȡ�
  * LED�ƺͼ�ʪ����
  * 3������ģʽ��16�������ݰ���ʽ�������ݣ���modbusЭ����д��䡣
  * ʾ����03 E9 17 D1 EA 17 45
  * ���У�03 E9ΪLoRa���ص�ַ��16�������֣�3E9תΪ10����Ϊ1001��17ΪLoRa�����ŵ���16
  * �������֣�ת10����Ϊ23��D1Ϊ�ڵ�1�ı�ʶ��EAΪDHT11�������ı�ʶ��17��45�ֱ�Ϊ��ʪ
  * �ȵ�16�������֡�
  ******************************************************************************
  */

typedef struct {
  uint8_t ucTemp;
  uint8_t ucHumi;
} DHT11DataID_t;

int main(void) {
  DHT11DataID_t xDHT11DataID;
  SysTick_Init(72);
  vLoRaUSART3EnableInit(115200);
  LED_PC13_Init();
  Fan_Relay_Init();
  while(1)
  {
    DHT11_Read_Data(&xDHT11DataID.ucTemp,&xDHT11DataID.ucHumi);
    #if LORA_MODE_INIT
      vLoRaNode1SendMsg();
      vLoRaNode1Executing();
      delay_ms(3000);
    #endif
  }
}

void vLoRaNode1SendMsg(void)
{
  DHT11DataID_t xDHT11DataID;
  vLoRaUSART3GateIdPkt();
  vLoRaUSART3SendArray(&xLoRaSensorID.ucDHT11,1);
  vLoRaUSART3SendArray(&xDHT11DataID.ucTemp,1);
  vLoRaUSART3SendArray(&xDHT11DataID.ucHumi,1);
}

void vLoRaNode1Executing(void)
{
  LoRaExecutorFlag_t xLoRaExecutorFlag;
  if(xLoRaExecutorFlag.ucState == 11)
  {
    LED_PC13_ON();
    vLoRaUSART3SendArray(&xLoRaExecutorID.ucLED,1);
    vLoRaUSART3SendArray(&xLoRaExecutorStatus.ucStatusOn,1);
  }
  else if(xLoRaExecutorFlag.ucState == 12)
  {
    LED_PC13_OFF();
    vLoRaUSART3SendArray(&xLoRaExecutorID.ucLED,1);
    vLoRaUSART3SendArray(&xLoRaExecutorStatus.ucStatusOff,1);
  }
  else if(xLoRaExecutorFlag.ucState == 21)
  {
    Fan_Relay_On();
    vLoRaUSART3SendArray(&xLoRaExecutorID.ucFan,1);
    vLoRaUSART3SendArray(&xLoRaExecutorStatus.ucStatusOn,1);
  }
  else if(xLoRaExecutorFlag.ucState == 22)
  {
    Fan_Relay_Off();
    vLoRaUSART3SendArray(&xLoRaExecutorID.ucFan,1);
    vLoRaUSART3SendArray(&xLoRaExecutorStatus.ucStatusOff,1);
  }
}

/*bug��1����һ��ִ��ָ�����������ڶ��λ���һ���ӳ٣������βŻ���Ƴɹ������ҵڶ��νڵ㷢�������ɡ�0xEA����Ϊ��0xD1�������ѽ���� 
 *     2�������ط�����һ��ָ��󣬽ڵ㷢�������쳣�����ѽ���� 
 */
