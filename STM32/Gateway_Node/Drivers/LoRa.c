#include "LoRa.h"

SerialReceiver_t xLoRaReceiver = {0};
LoRaMsgRecvData_t xLoRaMsgRecvData;

/**
  * @brief  串口3接收中断处理函数
  * @note   接收串口中断发送的每帧数据到串口消息队列中，以便后续处理数据。
  * @param  None
  * @retval None
  */
void vLoRaIrqHandler(void)
{
#if ENABLE_FREERTOS_API_QUEUE_USART3_IRQ
    if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
    {
        uint8_t ucRxData = (uint8_t)USART_ReceiveData(USART3);
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xQueueSendFromISR(xQueueUsart3IrqHdlr, &ucRxData, &xHigherPriorityTaskWoken);
        /* 问题根源：请求上下文切换 */ 
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
    }
#else
    uint8_t ucRxData;
    if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
    {
        ucRxData = USART_ReceiveData(USART3);
        if (xLoRaReceiver.Flags_t.usBufferIndex < (LORA_MAX_BUFFER - 2))                    // 预留1个字节写结束符
            xLoRaReceiver.cRecvBuffer[xLoRaReceiver.Flags_t.usBufferIndex++] = ucRxData;
        // USART_SendData(USART1, ucRxData);                                                   // 打印到指定串口调试
        if (xLoRaReceiver.Flags_t.usBufferIndex >= (LORA_MAX_BUFFER - 2))
        {
            xLoRaReceiver.Flags_t.usBufferIndex = 0;
            memset((uint8_t *)xLoRaReceiver.cRecvBuffer, 0, sizeof(xLoRaReceiver.cRecvBuffer));
        }
    }
    if (USART_GetITStatus(USART3, USART_IT_IDLE) == SET)                                    // 数据帧接收完毕
    {
        xLoRaReceiver.Flags_t.usRecvFinishFlag = 1;
    }
#endif
}

bool bLoRaSendATCmdForAck(char *pcCmd, char *pcAck1, char *pcAck2, uint32_t ulWaittime)
{
    xLoRaReceiver.Flags_t.usBufferIndex = 0;                                                // 从新开始接收新的数据包
    vLoRaDataTransmit("%s\r\n", pcCmd);
    if ((pcAck1 == 0) && (pcAck2 == 0))                                                     // 不需要接收数据
        return true;
    vDelayMs(ulWaittime);                                                                   // 延时
    xLoRaReceiver.cRecvBuffer[xLoRaReceiver.Flags_t.usBufferIndex] = '\0';
    if ((pcAck1 != 0) && (pcAck2 != 0))
        return ((bool)strstr(xLoRaReceiver.cRecvBuffer, pcAck1) ||
                (bool)strstr(xLoRaReceiver.cRecvBuffer, pcAck2));
    else if (pcAck1 != 0)
        return ((bool)strstr(xLoRaReceiver.cRecvBuffer, pcAck1));
    else
        return ((bool)strstr(xLoRaReceiver.cRecvBuffer, pcAck2));
}

void vLoRaConfigQuery(void)
{
    LORA_MD0_SETHIGHLVL();
    char cCmd[64] = {0};
    snprintf(cCmd, sizeof(cCmd), "AT+WLRATE?");
    if (bLoRaSendATCmdForAck(cCmd, "OK", NULL, 300) == true)
    {
        vDebugLogPrint("test ok\r\n");
    } 
    else
    {
        vDebugLogPrint("test error\r\n");
    }
    LORA_MD0_SETLOWLVL();
}

void vLoRaConnection(uint8_t ucNodeId)
{
    if (ucNodeId == xLoRaNodeConfig.Node1Cfg_t.ucLoRaNode1Identifier)
    {
        vLoRaHexNumTransmit((uint8_t *)&xLoRaNodeConfig.Node1Cfg_t, 3);
    }
    else if (ucNodeId == xLoRaNodeConfig.Node2Cfg_t.ucLoRaNode2Identifier)
    {
        vLoRaHexNumTransmit((uint8_t *)&xLoRaNodeConfig.Node2Cfg_t, 3);
    }
    else
    {
        vLoRaHexNumTransmit((uint8_t *)&xLoRaNodeConfig.GateCfg_t, 3);
    }
}

// 可抽象成：prev 上个状态 next 下个状态
#define LORA_FRAME_LENGTH   8   // 一帧固定长度

void ucLoRaRecvDataParse(uint8_t *pucBuffer, uint8_t ucLength)
{
    static enum
    {
        WAIT_HEAD,
        WAIT_NODEID,
        WAIT_SENSOR,
        WAIT_TEMP,
        WAIT_HUMIDITY,
        WAIT_TAIL        // 新增：处理温湿度之后的剩余字节
    } State_t = WAIT_HEAD;

    // 用来记录尾部还需跳过的字节数（对于8字节帧，尾部长3字节）
    static uint8_t ucTailRemain = 0;

    for (int i = 0; i < ucLength; i++)
    {
        uint8_t byte = pucBuffer[i];

        switch (State_t)
        {
        case WAIT_HEAD:
            if (byte == LORA_MSG_HEAD)
            {
                State_t = WAIT_NODEID;
            }
            else
            {
                vDebugLogPrint("Status 1 error!.\r\n");
            }
            break;

        case WAIT_NODEID:
            if (byte == LORA_NODE1_IDENTIFIER || byte == LORA_NODE2_IDENTIFIER)
            {
                State_t = WAIT_SENSOR;
            }
            break;

        case WAIT_SENSOR:
            if (byte == LORA_SENSOR_DHT11)
            {
                State_t = WAIT_TEMP;
            }
            break;

        case WAIT_TEMP:
            xLoRaMsgRecvData.Sensor_t.ucTemperature = byte;
            State_t = WAIT_HUMIDITY;
            break;

        case WAIT_HUMIDITY:
            xLoRaMsgRecvData.Sensor_t.ucHumidity = byte;
            // 进入尾部处理状态，需要跳过 (帧长 - 已处理字节数) 字节
            // 已处理：HEAD(1) + NODEID(1) + SENSOR(1) + TEMP(1) + HUMIDITY(1) = 5 字节
            ucTailRemain = LORA_FRAME_LENGTH - 5;   // 8 - 5 = 3 字节
            State_t = WAIT_TAIL;
            break;

        case WAIT_TAIL:
            // 跳过尾部字节
            if (ucTailRemain > 0)
            {
                ucTailRemain--;
                if (ucTailRemain == 0)
                {
                    State_t = WAIT_HEAD;    // 尾部结束，准备接收下一帧
                }
            }
            else
            {
                // 防御：若异常进入此处，直接回到 HEAD
                State_t = WAIT_HEAD;
            }
            break;

        default:
            State_t = WAIT_HEAD;
            break;
        }
    }
}

void vLoRaParseDataTest(void)
{
    ucLoRaRecvDataParse((uint8_t *)xLoRaReceiver.cRecvBuffer, xLoRaReceiver.Flags_t.usBufferIndex);
}
