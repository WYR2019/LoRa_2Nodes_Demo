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
  * @brief   主程序文件
  ******************************************************************************
  * @attention
  * 1、该项目为基于STM32、LoRa的室内环境监测系统，其中这是节点1的节点检测代码。
	* 2、LoRa数据采集节点1主要使用DHT11传感器采集温湿度，用户可在移动端远程控制风扇、
  * LED灯和加湿器。
  * 3、定点模式以16进制数据包格式传输数据，仿modbus协议进行传输。
  * 示例：03 E9 17 D1 EA 17 45
  * 其中：03 E9为LoRa网关地址的16进制数字，3E9转为10进制为1001；17为LoRa网关信道的16
  * 进制数字，转10进制为23；D1为节点1的标识，EA为DHT11传感器的标识，17、45分别为温湿
  * 度的16进制数字。
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

/*bug：1、第一次执行指令正常，但第二次会有一次延迟，第三次才会控制成功，而且第二次节点发送数据由“0xEA”变为“0xD1”。（已解决） 
 *     2、在网关发布第一次指令后，节点发送数据异常。（已解决） 
 */
