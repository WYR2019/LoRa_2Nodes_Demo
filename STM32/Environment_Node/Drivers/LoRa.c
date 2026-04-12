#include "LoRa.h"

/**
  * @brief  LoRa发送节点ID数据包封装函数。				
  * @note   LoRa在定点模式下执行节点间通信的主要函数。
  * @param  ucNodeId 节点ID，如Node1，也就是D1等等。
  * @retval None
  */
void vLoRaConnectionInit(uint8_t ucNodeId)
{
    if (ucNodeId == xLoRaNode1Config.ucLoRaNode1Identifier)
    {
        vLoRaSendArray((uint8_t *)&xLoRaNode1Config, 3);
    }
    else if (ucNodeId == xLoRaNode2Config.ucLoRaNode2Identifier)
    {
        vLoRaSendArray((uint8_t *)&xLoRaNode2Config, 3);
    }
    else if (ucNodeId == xLoRaGateConfig.ucLoRaGateIdentifier)
    {
        vLoRaSendArray((uint8_t *)&xLoRaGateConfig, 3);
    }
}

void vLoRaProtocolSend(uint8_t ucNodeId, uint8_t ucSensorId, uint8_t ucExeId,
                       uint8_t ucData1, uint8_t ucData2, uint8_t ucData3)
{
    uint8_t pucBuffer[8];                                                       // 使用局部数组代替野指针
    uint8_t ucSum = 0;

    pucBuffer[0] = 0xAA;                                                        // 帧头
    pucBuffer[1] = ucNodeId;
    pucBuffer[2] = ucSensorId;
    pucBuffer[3] = ucData1;
    pucBuffer[4] = ucData2;
    pucBuffer[5] = ucExeId;
    pucBuffer[6] = ucData3;

    /* 计算累加和（从索引0到6累加，或将帧头包含在内，按协议规定） */
    for (int i = 0; i < 7; i++)                                                 // 只累加前7个字节
    {
        ucSum += pucBuffer[i];
    }
    pucBuffer[7] = ucSum;                                                       // 存入校验和

    vLoRaSendArray(pucBuffer, 8);
}

/**
  * @brief  LoRa节点接收返回数据封装函数。				
  * @note   通过从串口消息队列接收什么数据判断是什么指令。
  * @param  *pucData          接收到的数据。
  * @retval eLoRaRecvStatus_t 枚举，包括节点ID、传感器ID、执行器ID和开关指令。
  */
eLoRaRecvStatus_t xLoRaMessageReceived(uint8_t *pucData)
{
    switch (*pucData)
    {
        case 0xD1:
        return statusNode1;
    
        case 0xD2:
        return statusNode2;

        case 0xFA:
        return statusExeLed;

        case 0xFB:
        return statusExeFan;

        case 0xFC:
        return statusExeHumidifier;

        case 0xFD:
        return statusExeBuzzer;

        case 0xFE:
        return statusExeServo;

        case 0x01:
        return statusExeOn;

        case 0x00:
        return statusExeOff;

        default:
        return null;
    }
}

/**
  * @brief  LoRa节点处理返回数据封装函数。				
  * @note   通过LoRa节点接收返回数据封装函数判断是什么指令，并细化处理，可减少代码复杂度。该API可直接被其他函数调用。
  * @param  ucDataRecNodeId    要判断的节点ID。
  * @param  ucDataRecExeId     要判断的执行器ID。
  * @param  ucDataRecExeStatus 要判断的传感器ID。
  * @retval eLoRaRecvStatus_t  枚举，包括节点ID、传感器ID、执行器ID和开关指令。
  */
eLoRaRecvStatus_t xLoRaMsgProcess(uint8_t ucDataRecNodeId, uint8_t ucDataRecExeId, uint8_t ucDataRecExeStatus)
{
    switch (xLoRaMessageReceived(&ucDataRecNodeId))
    {
        case statusNode1:
        while (xLoRaMessageReceived(&ucDataRecExeStatus) == statusExeOn)
        {
            if (xLoRaMessageReceived(&ucDataRecExeId) == statusExeLed)
            {
                return statusLedOn;
            } 
            else if (xLoRaMessageReceived(&ucDataRecExeId) == statusExeFan)
            {
                return statusFanOn;
            } 
            else if (xLoRaMessageReceived(&ucDataRecExeId) == statusExeHumidifier)
            {
                return statusHumidifierOn;
            }
        }
        while (xLoRaMessageReceived(&ucDataRecExeStatus) == statusExeOff)
        {
            if (xLoRaMessageReceived(&ucDataRecExeId) == statusExeLed)
            {
                return statusLedOff;
            } 
            else if (xLoRaMessageReceived(&ucDataRecExeId) == statusExeFan)
            {
                return statusFanOff;
            } 
            else if (xLoRaMessageReceived(&ucDataRecExeId) == statusExeHumidifier)
            {
                return statusHumidifierOff;
            }
        }
        
        case statusNode2:
        while (xLoRaMessageReceived(&ucDataRecExeStatus) == statusExeOn)
        {
            if (xLoRaMessageReceived(&ucDataRecExeId) == statusExeBuzzer)
            {
                return statusBuzzerOn;
            } 
            else if (xLoRaMessageReceived(&ucDataRecExeId) == statusExeServo)
            {
                return statusServoOn;
            }
        }
        while (xLoRaMessageReceived(&ucDataRecExeStatus) == statusExeOff)
        {
            if (xLoRaMessageReceived(&ucDataRecExeId) == statusExeBuzzer)
            {
                return statusBuzzerOff;
            } 
            else if (xLoRaMessageReceived(&ucDataRecExeId) == statusExeServo)
            {
                return statusServoOff;
            }
        }
        default:
        return null;
    }
}
