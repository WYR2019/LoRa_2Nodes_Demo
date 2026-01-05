#include "LoRa.h"

/**
  * @brief  LoRa发送节点ID数据包封装函数。				
  * @note   LoRa在定点模式下执行节点间通信的主要函数。
  * @param  ucNodeId 节点ID，如Node1，也就是D1等等。
  * @retval None
  */
void vLoRaConnectionPkt(uint8_t ucNodeId)
{
    if (ucNodeId == xLoRaNode1Config.ucLoRaNode1Identifier)
    {
        /* code */
        vUsartSendArray(USART3, (uint8_t *)&xLoRaNode1Config, 3);
    }
    else if (ucNodeId == xLoRaNode2Config.ucLoRaNode2Identifier)
    {
        /* code */
        vUsartSendArray(USART3, (uint8_t *)&xLoRaNode2Config, 3);
    }
    else
    {
        /* code */
        vUsartSendArray(USART3, (uint8_t *)&xLoRaGateConfig, 3);
    }
}
    
/**
  * @brief  LoRa发送节点ID数据包封装函数。				
  * @note   通过发送节点ID以声明自己是什么节点。
  * @param  ucNodeId 节点ID，如Node1，也就是D1等等。
  * @retval None
  */
void vLoRaToGateIdPkt(uint8_t ucNodeId)
{
    if (ucNodeId == xLoRaNode1Config.ucLoRaNode1Identifier)
    {
        /* code */
        vUsartSendArray(USART3, (uint8_t *)&xLoRaNode1Config.ucLoRaNode1Identifier, 1);
    } else if (ucNodeId == xLoRaNode2Config.ucLoRaNode2Identifier)
    {
        /* code */
        vUsartSendArray(USART3, (uint8_t *)&xLoRaNode2Config.ucLoRaNode2Identifier, 1);
    }
}

/**
  * @brief  LoRa发送传感器ID数据包封装函数。				
  * @note   通过发送传感器ID以声明自己是什么传感器。
  * @param  ucSensorId 传感器ID，如DHT11温湿度传感器，也就是ucIdDht11等等。
  * @retval None
  */
void vLoRaToGateSenIdPkt(uint8_t ucSensorId)
{
    if (ucSensorId == xLoRaSensorID.ucIdDht11)
    {
        /* code */
        vUsartSendArray(USART3, (uint8_t *)&xLoRaSensorID.ucIdDht11, 1);
    } else if (ucSensorId == xLoRaSensorID.ucIdMq2)
    {
        /* code */
        vUsartSendArray(USART3, (uint8_t *)&xLoRaSensorID.ucIdMq2, 1);
    } else if (ucSensorId == xLoRaSensorID.ucIdFire)
    {
        /* code */
        vUsartSendArray(USART3, (uint8_t *)&xLoRaSensorID.ucIdFire, 1);  
    } else if (ucSensorId == xLoRaSensorID.ucIdLight)
    {
        /* code */
        vUsartSendArray(USART3, (uint8_t *)&xLoRaSensorID.ucIdLight, 1);
    }
}

/**
  * @brief  LoRa发送执行器ID数据包封装函数。				
  * @note   通过发送执行器ID以声明自己是什么执行器。
  * @param  ucExeId 执行器ID，如LED灯，也就是ucIdLed等等。
  * @retval None
  */
void vLoRaToGateExeIdPkt(uint8_t ucExeId)
{
    if (ucExeId == xLoRaExecutorID.ucIdLed)
    {
        /* code */
        vUsartSendArray(USART3, (uint8_t *)&xLoRaExecutorID.ucIdLed, 1);
    } else if (ucExeId == xLoRaExecutorID.ucIdFan)
    {
        /* code */
        vUsartSendArray(USART3, (uint8_t *)&xLoRaExecutorID.ucIdFan, 1);
    } else if (ucExeId == xLoRaExecutorID.ucIdHumidifier)
    {
        /* code */
        vUsartSendArray(USART3, (uint8_t *)&xLoRaExecutorID.ucIdHumidifier, 1);
    } else if (ucExeId == xLoRaExecutorID.ucIdBuzzer)
    {
        /* code */
        vUsartSendArray(USART3, (uint8_t *)&xLoRaExecutorID.ucIdBuzzer, 1);
    } else if (ucExeId == xLoRaExecutorID.ucIdServo)
    {
        /* code */
        vUsartSendArray(USART3, (uint8_t *)&xLoRaExecutorID.ucIdServo, 1);
    }
}

/**
  * @brief  LoRa节点接收返回数据封装函数。				
  * @note   通过从串口消息队列接收什么数据判断是什么指令。
  * @param  *pucData          接收到的数据。
  * @retval eLoRaMsgRecStatus 枚举，包括节点ID、传感器ID、执行器ID和开关指令。
  */
eLoRaMsgRecStatus xLoRaMessageReceived(uint8_t *pucData)
{
    switch (*pucData)
    {
        case 0xD1:
        /* code */
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
  * @retval eLoRaMsgRecStatus  枚举，包括节点ID、传感器ID、执行器ID和开关指令。
  */
eLoRaMsgRecStatus xLoRaMsgProcess(uint8_t ucDataRecNodeId, uint8_t ucDataRecExeId, uint8_t ucDataRecExeStatus)
{
    switch (xLoRaMessageReceived(&ucDataRecNodeId))
    {
        case statusNode1:
        /* code */
        while (xLoRaMessageReceived(&ucDataRecExeStatus) == statusExeOn)
        {
            /* code */
            if (xLoRaMessageReceived(&ucDataRecExeId) == statusExeLed)
            {
                /* code */
                return statusLedOn;
            } else if (xLoRaMessageReceived(&ucDataRecExeId) == statusExeFan)
            {
                /* code */
                return statusFanOn;
            } else if (xLoRaMessageReceived(&ucDataRecExeId) == statusExeHumidifier)
            {
                /* code */
                return statusHumidifierOn;
            }
        }
        while (xLoRaMessageReceived(&ucDataRecExeStatus) == statusExeOff)
        {
            /* code */
            if (xLoRaMessageReceived(&ucDataRecExeId) == statusExeLed)
            {
                /* code */
                return statusLedOff;
            } else if (xLoRaMessageReceived(&ucDataRecExeId) == statusExeFan)
            {
                /* code */
                return statusFanOff;
            } else if (xLoRaMessageReceived(&ucDataRecExeId) == statusExeHumidifier)
            {
                /* code */
                return statusHumidifierOff;
            }
        }
        
        case statusNode2:
        while (xLoRaMessageReceived(&ucDataRecExeStatus) == statusExeOn)
        {
            /* code */
            if (xLoRaMessageReceived(&ucDataRecExeId) == statusExeBuzzer)
            {
                /* code */
                return statusBuzzerOn;
            } else if (xLoRaMessageReceived(&ucDataRecExeId) == statusExeServo)
            {
                /* code */
                return statusServoOn;
            }
        }
        while (xLoRaMessageReceived(&ucDataRecExeStatus) == statusExeOff)
        {
            /* code */
            if (xLoRaMessageReceived(&ucDataRecExeId) == statusExeBuzzer)
            {
                /* code */
                return statusBuzzerOff;
            } else if (xLoRaMessageReceived(&ucDataRecExeId) == statusExeServo)
            {
                /* code */
                return statusServoOff;
            }
        }
        default:
        return null;
    }
}
