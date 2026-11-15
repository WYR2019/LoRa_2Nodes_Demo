#ifndef __LORA_H__
#define __LORA_H__

#include <stdbool.h>
#include "BSP_Delay.h"
#include "BSP_GPIO.h"
#include "BSP_USART.h"
#include "App_LinearBuffer.h"
#include "LoRa_cfg.h"

/* LoRa的静态设置 */
#define loRaIrqHandler(void)                                USART3_IRQHandler(void)
#define loRaDataTransmit(format, ...)                       usartPrintf(USART3, format, ##__VA_ARGS__)
#define loRaHexNumTransmit(array, length)                   usartSendArray(USART3, array, length)
#define LORA_MD0_SETHIGHLVL()                               GPIO_SetBits(LORA_MD0_PORT, LORA_MD0_PIN)
#define LORA_MD0_SETLOWLVL()                                GPIO_ResetBits(LORA_MD0_PORT, LORA_MD0_PIN)

typedef struct
{
    struct
    {
        bool Node1OnlineState : 1;
        bool Node2OnlineState : 1;
        bool GateOnlineState : 1;
    } NodeOnlineState_t;
    struct
    {
        uint8_t temperature;
        uint8_t humidity;
    } SensorData_t;
    struct
    {
        uint8_t lightStatus;
        uint8_t fanStatus;
        uint8_t humidifierStatus;
        uint8_t tempLow;
        uint8_t tempMax;
        uint8_t humiMin;
        uint8_t humiMax;
    } ExecutorData_t;
} LoRaRecvData_t;

void loRaHeaderFrameSend(uint8_t ucNodeId);
void receiveFromNodes(LoRaRecvData_t *loRaRecvData);

#endif
