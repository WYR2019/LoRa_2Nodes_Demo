#include "LoRa.h"

void vLoRaConnectionPkt(uint8_t ucNodeId)
{
    if (ucNodeId == xLoRaNode1Config.ucLoRaNode1Identifier)
    {
        /* code */
        vUsart3SendArray((uint8_t *)&xLoRaNode1Config, 3);
    }
    else if (ucNodeId == xLoRaNode2Config.ucLoRaNode2Identifier)
    {
        /* code */
        vUsart3SendArray((uint8_t *)&xLoRaNode2Config, 3);
    }
    else
    {
        /* code */
        vUsart3SendArray((uint8_t *)&xLoRaGateConfig, 3);
    }
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

void vLoRaToGateExeIdPkt(uint8_t ucExeId)
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
        return statusNode1;
    }
}

eLoRaMsgRecStatus xLoRaMsgProcess(uint8_t ucDataRecNodeId, uint8_t ucDataRecExeId, uint8_t ucDataRecExeSta)
{
    switch (xLoRaMessageReceived(&ucDataRecNodeId))
    {
        case statusNode1:
        /* code */
        while (xLoRaMessageReceived(&ucDataRecExeSta) == statusExeOn)
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
        while (xLoRaMessageReceived(&ucDataRecExeSta) == statusExeOff)
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
        while (xLoRaMessageReceived(&ucDataRecExeSta) == statusExeOn)
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
        while (xLoRaMessageReceived(&ucDataRecExeSta) == statusExeOff)
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
        return statusNode1;
    }
}
