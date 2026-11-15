#include "LoRa.h"

LinearBuffer_t loRaReceiver = {0};

/**
  * @brief  串口3接收中断处理函数
  * @note   接收串口中断发送的每帧数据到串口消息队列中，以便后续处理数据。
  * @param  None
  * @retval None
  */
void loRaIrqHandler(void)
{
#if ENABLE_FREERTOS_API_QUEUE_USART3_IRQ
    if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
    {
        uint8_t rxData = (uint8_t)USART_ReceiveData(USART3);
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xQueueSendFromISR(queueUsart3IrqHdlr, &rxData, &xHigherPriorityTaskWoken);
        /* 问题根源：请求上下文切换 */ 
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
    }
#else
    uint8_t rxData;
    if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
    {
        rxData = USART_ReceiveData(USART3);
        writeToBuffer(&loRaReceiver, rxData);
        traceFromIrq(rxData);                                                                 // 打印到指定串口调试
    }
    if (USART_GetITStatus(USART3, USART_IT_IDLE) == SET)                                    // 数据帧接收完毕
    {
        rxData = USART_ReceiveData(USART3);                                                 // 由软件序列清除中断标志位(先读USART_SR，然后读USART_DR)
    }
#endif
}

void loRaHeaderFrameSend(uint8_t ucNodeId)
{
    switch (ucNodeId)
    {
    case LORA_NODE1_IDENTIFIER:
        loRaHexNumTransmit((uint8_t *)&loRaNodeConfig.Node1Cfg_t, 3);
        break;

    case LORA_NODE2_IDENTIFIER:
        loRaHexNumTransmit((uint8_t *)&loRaNodeConfig.Node2Cfg_t, 3);
        break;

    default:
        loRaHexNumTransmit((uint8_t *)&loRaNodeConfig.GateCfg_t, 3);
        break;
    }
}

// 可抽象成：prev 上个状态 next 下个状态

/*
 * 该函数的作用是接收LoRa节点发送的数据，并解析出传感器和执行器的数据，存储在loRaRecvData结构体中，以便后续处理。
 * 缓冲区内容举例：
 * [0] [1] [2] [3] [4] [5] [6] [7] [8] [9] [10] [11] [12] [13] [14] [15] [16] [17]
 * AA  A1  B1  1D  39  C1  00  13  AB  AA   A1   B1   1C   3C   C1   00   15   AB
 * AA : 帧头
 * A1 : 节点标识符
 * B1 : 传感器DHT11标识符
 * 1D : 温度数据
 * 39 : 湿度数据
 * C1 : 执行器LED标识符
 * 00 : 执行器LED状态数据，00表示关闭，01表示开启
 * 13 : 校验和
 * AB : 帧尾
 * 状态机实现原理：先找到帧头AA和帧尾AB，将这段报文复制到buffer中并进行校验，若校验值正确则进行下一步的判断。
 * 然后根据节点标识符A1，找到对应的传感器和执行器数据，最后根据校验和验证数据的完整性。
 * 如何找到帧头AA：使用strrchr函数从缓冲区中查找最后一个AA的位置，确保获取到最新的一帧数据。
 * 如何根据节点标识符A1找到对应的传感器和执行器数据：根据节点标识符A1，判断是哪个节点的数据，然后根据传感器和执行器的标识符，提取对应的数据。
 * 如何根据校验和验证数据的完整性：将数据帧中除校验和外的所有字节相加并取最后两位，得到的结果与校验和进行比较，如果相等，则数据完整，否则数据有误。
 */
void receiveFromNodes(LoRaRecvData_t *loRaRecvData)
{
    char buffer[16] = {0};
    int updateFrameHeaderIndex = 0, updateFrameTailIndex = 0;
    for (uint16_t i = 0; i < loRaReceiver.bufferIndex; i++)
    {
        if (updateFrameHeaderIndex > loRaReceiver.bufferIndex)
        {
            updateFrameHeaderIndex = 0;                                                                                 // 如果帧头索引大于缓冲区索引，则重置帧头索引
        }
        if ((uint8_t)loRaReceiver.buffer[i] == 0xAA)                                                                    // 帧头
        {
            updateFrameHeaderIndex = i;                                                                                 // 找到最后一个帧头
        }
    }
    for (uint16_t i = 0; i < loRaReceiver.bufferIndex; i++)
    {
        if (updateFrameTailIndex > loRaReceiver.bufferIndex)
        {
            updateFrameTailIndex = 0;                                                                                   // 如果帧尾索引大于缓冲区索引，则重置帧尾索引
        }
        if ((uint8_t)loRaReceiver.buffer[i] == 0xAB)                                                                    // 帧尾
        {
            updateFrameTailIndex = i;
        }
    }
    memcpy(buffer, loRaReceiver.buffer + updateFrameHeaderIndex, updateFrameTailIndex - updateFrameHeaderIndex + 1);    // 加上偏移量
    trace("Buffer: ");
    for (uint8_t i = 0; i < 16; i++)
    {
        trace("%02X ", buffer[i]);
    }
    trace("\r\n");

    if ((uint8_t)buffer[0] == 0xAA && (uint8_t)buffer[updateFrameTailIndex - updateFrameHeaderIndex] == 0xAB)           // 判断帧头和帧尾是否正确
    {
        uint8_t checksum = 0;
        for (uint8_t i = 0; i < updateFrameTailIndex - updateFrameHeaderIndex - 1; i++)                                 // 校验和计算，排除帧头和帧尾
        {
            checksum += (uint8_t)buffer[i];
        }
        if (checksum == (uint8_t)buffer[updateFrameTailIndex - updateFrameHeaderIndex - 1])                             // 校验和验证
        {
            for (int i = 0; i < updateFrameTailIndex - updateFrameHeaderIndex - 2; i++)
            {
                if ((uint8_t)buffer[i] == LORA_NODE1_IDENTIFIER)                                                        // 节点1
                {
                    loRaRecvData->NodeOnlineState_t.Node1OnlineState = true;
                    for (int j = 0; j < updateFrameTailIndex - updateFrameHeaderIndex - 3; j++)
                    {
                        if ((uint8_t)buffer[j] == SENSORID_DHT11)                                                       // 传感器标识符
                        {
                            loRaRecvData->SensorData_t.temperature = (uint8_t)buffer[j + 1];                            // 温度数据
                            loRaRecvData->SensorData_t.humidity = (uint8_t)buffer[j + 2];                               // 湿度数据
                        }
                        if ((uint8_t)buffer[j] == EXECUTORID_LED)                                                       // 执行器标识符
                        {
                            loRaRecvData->ExecutorData_t.lightStatus = (uint8_t)buffer[j + 1];                          // LED状态数据
                        }
                        if ((uint8_t)buffer[j] == EXECUTORID_FAN)                                                       // 执行器标识符
                        {
                            loRaRecvData->ExecutorData_t.fanStatus = (uint8_t)buffer[j + 1];                            // 风扇状态数据
                        }
                        if ((uint8_t)buffer[j] == EXECUTORID_HUMIDIFIER)                                                // 执行器标识符
                        {
                            loRaRecvData->ExecutorData_t.humidifierStatus = (uint8_t)buffer[j + 1];                     // 加湿器状态数据
                        }
                    }
                } else if ((uint8_t)buffer[i] == LORA_NODE2_IDENTIFIER)                                                 // 节点2
                {
                    loRaRecvData->NodeOnlineState_t.Node2OnlineState = true;
                    for (int j = 0; j < updateFrameTailIndex - updateFrameHeaderIndex - 3; j++)
                    {
                        if ((uint8_t)buffer[j] == SENSORID_MQ2)
                        {
                            /* code */
                        }
                    }
                }
            }
        }
    }
    memset(buffer, 0, sizeof(buffer));
}
