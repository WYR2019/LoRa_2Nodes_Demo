#include "LoRa.h"

void vLoRaConnectionPkt(void)
{
    vUsart3SendArray((uint8_t *)&xLoRaGateConfig, 3);
}

/**
  * @brief  LoRa发送发送hex数据包函数,也是LoRa在定点模式下执行节点间通信的主要函数。				
  * @note   hex数据包的格式为固定包长
  * @param  None
  * @retval None
  */
void vLoRaToGateIdPkt(uint8_t ucNodeId)
{
    if (ucNodeId == xLoRaNode1Config.ucLoRaNode1Identifier)
    {
        /* code */
        vUsart3SendArray((uint8_t *)&xLoRaNode1Config.ucLoRaNode1Identifier, 1);
    } else if (ucNodeId == xLoRaNode2Config.ucLoRaNode2Identifier)
    {
        /* code */
        vUsart3SendArray((uint8_t *)&xLoRaNode2Config.ucLoRaNode2Identifier, 1);
    }
}

void vLoRaToGateSenIdPkt(uint8_t ucSensorId)
{
    if (ucSensorId == xLoRaSensorID.ucIdDht11)
    {
        /* code */
        vUsart3SendArray((uint8_t *)&xLoRaSensorID.ucIdDht11, 1);
    } else if (ucSensorId == xLoRaSensorID.ucIdMq2)
    {
        /* code */
        vUsart3SendArray((uint8_t *)&xLoRaSensorID.ucIdMq2, 1);
    } else if (ucSensorId == xLoRaSensorID.ucIdFire)
    {
        /* code */
        vUsart3SendArray((uint8_t *)&xLoRaSensorID.ucIdFire, 1);  
    } else if (ucSensorId == xLoRaSensorID.ucIdLight)
    {
        /* code */
        vUsart3SendArray((uint8_t *)&xLoRaSensorID.ucIdLight, 1);
    }
}

void vLoRToGateExeIdPkt(uint8_t ucExeId)
{
    if (ucExeId == xLoRaExecutorID.ucIdLed)
    {
        /* code */
        vUsart3SendArray((uint8_t *)&xLoRaExecutorID.ucIdLed, 1);
    } else if (ucExeId == xLoRaExecutorID.ucIdFan)
    {
        /* code */
        vUsart3SendArray((uint8_t *)&xLoRaExecutorID.ucIdFan, 1);
    } else if (ucExeId == xLoRaExecutorID.ucIdHumidifier)
    {
        /* code */
        vUsart3SendArray((uint8_t *)&xLoRaExecutorID.ucIdHumidifier, 1);
    } else if (ucExeId == xLoRaExecutorID.ucIdBuzzer)
    {
        /* code */
        vUsart3SendArray((uint8_t *)&xLoRaExecutorID.ucIdBuzzer, 1);
    } else if (ucExeId == xLoRaExecutorID.ucIdServo)
    {
        /* code */
        vUsart3SendArray((uint8_t *)&xLoRaExecutorID.ucIdServo, 1);
    }
}
