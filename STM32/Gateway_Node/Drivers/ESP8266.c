#include "ESP8266.h"

/* 初始化串口缓冲区句柄 */
SerialReceiver_t xEsp8266Receiver = {0};

/**
  * @brief  vEsp8266Rst
  * @note   调用这个函数，就可以重启ESP8266模块，该函数被vEsp8266AtTest调用。
  * @param  无
  * @retval 无
  */
void vEsp8266Rst(void)
{
#if 0
    bEsp8266Command("AT+RST", "OK", "ready", 2500);
#else
    ESP8266_RST_LOW_LEVEL();
    vDelayMs(500);
    ESP8266_RST_HIGH_LEVEL();
#endif
}

/**
  * @brief  串口2接收中断处理函数
  * @note   接收串口中断发送的每帧数据到串口消息队列中，以便后续处理数据。
  * @param  None
  * @retval None
  */
void vEsp8266IrqHandler(void)
{
    uint8_t ucRxData = 0;
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        ucRxData = USART_ReceiveData(USART2);
        if (xEsp8266Receiver.Flags_t.usBufferIndex < (BUFFER_MAX_LENGTH - 1))                                   // 预留1个字节写结束符
            xEsp8266Receiver.cRecvBuffer[xEsp8266Receiver.Flags_t.usBufferIndex++] = ucRxData;
        // USART_SendData(USART1, ucRxData);                                                                       // 打印到指定串口调试
    }
    if (USART_GetITStatus(USART2, USART_IT_IDLE) == SET)                                                        // 数据帧接收完毕
    {
        xEsp8266Receiver.Flags_t.usRecvFinishFlag = 1;
        ucRxData = USART_ReceiveData(USART2);                                                                   // 由软件序列清除中断标志位(先读USART_SR，然后读USART_DR)
    }
}

/**
 * @brief  bEsp8266Command
 * @note   对ESP8266模块发送AT指令,并等待响应。该函数被外部调用。
 * @param  pcCmd：待发送的指令
 * @param  pcAck1
 * @param  pcAck2：期待的响应，为NULL表不需响应，两者为或逻辑关系。
 * @param  ulWaittime：等待响应的时间
 * @retval true：指令发送成功
 * @retval false：指令发送失败
 */
bool bEsp8266Command(char *pcCmd, char *pcAck1, char *pcAck2, uint32_t ulWaittime)
{
    xEsp8266Receiver.Flags_t.usBufferIndex = 0;                                          // 从新开始接收新的数据包
    vEsp8266DataTransmit("%s\r\n", pcCmd);
    if ((pcAck1 == 0) && (pcAck2 == 0))                                                     // 不需要接收数据
        return true;
    vDelayMs(ulWaittime);                                                                   // 延时
    xEsp8266Receiver.cRecvBuffer[xEsp8266Receiver.Flags_t.usBufferIndex] = '\0';
    if ((pcAck1 != 0) && (pcAck2 != 0))
        return ((bool)strstr(xEsp8266Receiver.cRecvBuffer, pcAck1) ||
                (bool)strstr(xEsp8266Receiver.cRecvBuffer, pcAck2));
    else if (pcAck1 != 0)
        return ((bool)strstr(xEsp8266Receiver.cRecvBuffer, pcAck1));
    else
        return ((bool)strstr(xEsp8266Receiver.cRecvBuffer, pcAck2));
}

/**
  * @brief  vEsp8266AtTest
  * @note   对ESP8266模块进行AT测试启动，该函数被外部调用。
  * @param  无
  * @retval 无
  */
void vEsp8266AtTest(void)
{
    int iCnt = 0;
    ESP8266_RST_HIGH_LEVEL();
    vDelayMs(1000);
    while (iCnt < 3)
    {
        if (bEsp8266Command("AT", "OK", NULL, 500) != true)
            return;
        vEsp8266Rst();
        iCnt++;
    }
}

/** 
  * @brief  bEsp8266NetModeChoose
  * @note   选择ESP8266模块的工作模式,该函数被外部调用。
  * @param  xMode，工作模式
  * @retval 1，选择成功
  * @retval 0，选择失败
  */
bool bEsp8266NetModeChoose(eNetMode_t xMode)
{
    switch (xMode)
    {
    case STA:
        return bEsp8266Command("AT+CWMODE=1", "OK", "no change", 2500);
    case AP:
        return bEsp8266Command("AT+CWMODE=2", "OK", "no change", 2500);
    case STA_AP:
        return bEsp8266Command("AT+CWMODE=3", "OK", "no change", 2500);
    default:
        return false;
    }
}

/**
  * @brief  bEsp8266JoinAp
  * @note   ESP8266模块连接外部WiFi，该函数被外部调用。
  * @param  pcSsid，WiFi名称字符串
  * @param  pcPassWord，WiFi密码字符串
  * @retval 1，连接成功
  * @retval 0，连接失败
  */
bool bEsp8266JoinAp(char *pcSsid, char *pcPassWord)
{
    char cCmd[128] = {0};
    sprintf(cCmd, "AT+CWJAP=\"%s\",\"%s\"", pcSsid, pcPassWord);
    if (strstr(xEsp8266Receiver.cRecvBuffer, "WIFI CONNECTED") != NULL)
    {
        return true;
    }
    return bEsp8266Command(cCmd, "OK", "WIFI CONNECTED", 8000);
}

/**
  * @brief  bEsp8266MqttInit
  * @note   ESP8266模块MQTT初始化，该函数被外部调用。
  * @param  pcMqttUserName，MQTT用户名字符串
  * @param  pcMqttPassword，MQTT密码字符串
  * @param  pcMqttClientId，MQTT客户端ID字符串
  * @param  pcMqttServerIp，MQTT服务器IP地址字符串
  * @param  usMqttServerPort，MQTT服务器端口号
  * @retval 1，初始化成功
  * @retval 0，初始化失败
  */
bool bEsp8266MqttInit(char *pcMqttUserName, char *pcMqttPassword, char *pcMqttClientId, char *pcMqttServerIp,
                      uint16_t usMqttServerPort, char *pcMqttSubscribeTopic)
{
    char cCmd[256] = {0};

    /* 1. 配置MQTT用户信息 */
    snprintf(cCmd, sizeof(cCmd), "AT+MQTTUSERCFG=0,1,\"%s\",\"%s\",\"%s\",0,0,\"\"", pcMqttClientId, pcMqttUserName, pcMqttPassword);
    if (bEsp8266Command(cCmd, "OK", NULL, 2000) != true)
        return false;

    /* 2. 连接到MQTT服务器 */
    memset(cCmd, 0, sizeof(cCmd));
    snprintf(cCmd, sizeof(cCmd), "AT+MQTTCONN=0,\"%s\",%u,1", pcMqttServerIp, usMqttServerPort);
    if (bEsp8266Command(cCmd, "OK", NULL, 8000) != true && bEsp8266Command(cCmd, "+MQTTCONN", NULL, 8000) != true)
        return false;

    /* 3. 订阅主题 */
    memset(cCmd, 0, sizeof(cCmd));
    snprintf(cCmd, sizeof(cCmd), "AT+MQTTSUB=0,\"%s\",1", pcMqttSubscribeTopic);
    if (bEsp8266Command(cCmd, "OK", NULL, 2000) != true)
        return false;

    return true;
}

/**
  * @brief  bMqttPublish
  * @note   通过ESP8266模块向MQTT服务器发布消息，该函数被外部调用。
  * @param  pcTopic：发布的主题字符串
  * @param  pcMessage：发布的消息字符串
  * @retval true：发布成功
  * @retval false：发布失败
  */
bool bMqttPublish(char *pcTopic, char *pcMessage)
{
    char cCmd[128] = {0};
    uint8_t ucLen = strlen(pcMessage);
    snprintf(cCmd, sizeof(cCmd), "AT+MQTTPUBRAW=0,\"%s\",%d,1,0", pcTopic, ucLen);
    return bEsp8266Command(cCmd, ">", NULL, 2000) && bEsp8266Command(pcMessage, "OK", NULL, 2000);
}

/**
  * @brief  bMqttSubscribe
  * @note   解析MQTT订阅消息，该函数被外部调用。
  * @param  pcTopic：订阅的主题字符串
  * @param  pcMessage：订阅的消息字符串
  * @retval true：解析成功
  * @retval false：解析失败
  */
bool bMqttSubscribe(char *pcTopic, char *pcMessage)
{
    /* 此函数用于解析订阅消息，根据不同的模式提取主题和消息内容，具体实现根据实际的消息格式进行编写。 */
    char *pcSubRecMsg = 0;
    pcSubRecMsg = strstr(xEsp8266Receiver.cRecvBuffer, pcMessage);
    if (strstr(xEsp8266Receiver.cRecvBuffer, pcTopic), pcSubRecMsg)
        return true;
    return false;                                                       // 占位符，实际实现需要返回解析结果
}
