#include "ESP8266.h"

/* 初始化串口缓冲区句柄 */
LinearBuffer_t wifiReceiver = {0};

/**
  * @brief  esp8266Reset
  * @note   调用这个函数，就可以重启ESP8266模块，该函数被esp8266AtTest调用。
  * @param  无
  * @retval 无
  */
void esp8266Reset(void)
{
#if 1
    isATCommandExists("AT+RST", "OK", "ready", 2500);
#else
    ESP8266_RST_LOW_LEVEL();
    delayMs(500);
    ESP8266_RST_HIGH_LEVEL();
#endif
}

/**
  * @brief  串口2接收中断处理函数
  * @note   接收串口中断发送的每帧数据到串口消息队列中，以便后续处理数据。
  * @param  None
  * @retval None
  */
void esp8266IrqHandler(void)
{
    uint8_t rxData = 0;
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        rxData = USART_ReceiveData(USART2);
        writeToBuffer(&wifiReceiver, rxData);
        // traceFromIrq(rxData);                                                                  // 打印到指定串口调试
    }
    if (USART_GetITStatus(USART2, USART_IT_IDLE) == SET)                                        // 数据帧接收完毕
    {
        rxData = USART_ReceiveData(USART2);                                                     // 由软件序列清除中断标志位(先读USART_SR，然后读USART_DR)
        wifiReceiver.buffer[wifiReceiver.bufferIndex] = '\0';
    }
}

/**
 * @brief  isATCommandExists
 * @note   对ESP8266模块发送AT指令,并等待响应。该函数被外部调用。
 * @param  cmd：待发送的指令
 * @param  ack1
 * @param  ack2：期待的响应，为NULL表不需响应，两者为或逻辑关系。
 * @param  waitTime：等待响应的时间
 * @retval true：指令发送成功
 * @retval false：指令发送失败
 */
bool isATCommandExists(char *cmd, char *ack1, char *ack2, uint32_t waitTime)
{
    resetBufferIndex(&wifiReceiver);
    esp8266DataTransmit("%s\r\n", cmd);
    if ((ack1 == 0) && (ack2 == 0))                                                             // 不需要接收数据
        return true;
    delayMs(waitTime);                                                                          // 延时
    if ((ack1 != 0) && (ack2 != 0))
        return ((bool)strstr(wifiReceiver.buffer, ack1) ||
                (bool)strstr(wifiReceiver.buffer, ack2));
    else if (ack1 != 0)
        return ((bool)strstr(wifiReceiver.buffer, ack1));
    else
        return ((bool)strstr(wifiReceiver.buffer, ack2));
}

/**
  * @brief  esp8266AtTest
  * @note   对ESP8266模块进行AT测试启动，该函数被外部调用。
  * @param  无
  * @retval 无
  */
void esp8266AtTest(void)
{
    int count = 0;
    ESP8266_RST_HIGH_LEVEL();
    delayMs(1000);
    while (count < 3)
    {
        if (isATCommandExists("AT", "OK", NULL, 500) != true)
            return;
        esp8266Reset();
        count++;
    }
}

/** 
  * @brief  esp8266NetModeChoose
  * @note   选择ESP8266模块的工作模式,该函数被外部调用。
  * @param  mode，工作模式
  * @retval 1，选择成功
  * @retval 0，选择失败
  */
bool esp8266NetModeChoose(NetMode_t mode)
{
    switch (mode)
    {
    case STA:
        return isATCommandExists("AT+CWMODE=1", "OK", "no change", 2500);
    case AP:
        return isATCommandExists("AT+CWMODE=2", "OK", "no change", 2500);
    case STA_AP:
        return isATCommandExists("AT+CWMODE=3", "OK", "no change", 2500);
    default:
        return false;
    }
}

/**
  * @brief  esp8266JoinAp
  * @note   ESP8266模块连接外部WiFi，该函数被外部调用。
  * @param  ssid，WiFi名称字符串
  * @param  password，WiFi密码字符串
  * @retval 1，连接成功
  * @retval 0，连接失败
  */
bool esp8266JoinAp(char *ssid, char *password)
{
    char cmd[128] = {0};
    sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"", ssid, password);
    if (strstr(wifiReceiver.buffer, "WIFI CONNECTED") != NULL)
    {
        return true;
    }
    return isATCommandExists(cmd, "OK", "WIFI CONNECTED", 8000);
}

/**
  * @brief  esp8266MqttInit
  * @note   ESP8266模块MQTT初始化，该函数被外部调用。
  * @param  mqttUsername，MQTT用户名字符串
  * @param  mqttPassword，MQTT密码字符串
  * @param  mqttClientId，MQTT客户端ID字符串
  * @param  mqttServerIp，MQTT服务器IP地址字符串
  * @param  mqttServerPort，MQTT服务器端口号
  * @param  mqttSubscribeTopic, MQTT订阅topic
  * @retval 1，初始化成功
  * @retval 0，初始化失败
  */
bool esp8266MqttInit(char *mqttUsername, char *mqttPassword, char *mqttClientId, 
    char *mqttServerIp, uint16_t mqttServerPort, char *mqttSubscribeTopic)
{
    char cmd[256] = {0};

    /* 1. 配置MQTT用户信息 */
    snprintf(cmd, sizeof(cmd), "AT+MQTTUSERCFG=0,1,\"%s\",\"%s\",\"%s\",0,0,\"\"",
        mqttClientId, mqttUsername, mqttPassword);
    if (isATCommandExists(cmd, "OK", NULL, 2000) != true)
        return false;

    /* 2. 连接到MQTT服务器 */
    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "AT+MQTTCONN=0,\"%s\",%u,1", 
        mqttServerIp, mqttServerPort);
    if (isATCommandExists(cmd, "OK", NULL, 8000) != true && 
        isATCommandExists(cmd, "+MQTTCONN", NULL, 8000) != true)
        return false;

    /* 3. 订阅主题 */
    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "AT+MQTTSUB=0,\"%s\",1", mqttSubscribeTopic);
    if (isATCommandExists(cmd, "OK", NULL, 2000) != true)
        return false;

    return true;
}

/**
  * @brief  mqttPublish
  * @note   通过ESP8266模块向MQTT服务器发布消息，该函数被外部调用。
  * @param  topic：发布的主题字符串
  * @param  message：发布的消息字符串
  * @retval true：发布成功
  * @retval false：发布失败
  */
bool mqttPublish(char *topic, char *message)
{
    char cmd[128] = {0};
    uint8_t length = strlen(message);
    snprintf(cmd, sizeof(cmd), "AT+MQTTPUBRAW=0,\"%s\",%d,1,0", topic, length);
    return isATCommandExists(cmd, ">", NULL, 2000)
        && isATCommandExists(message, "OK", NULL, 2000);
}

/**
  * @brief  mqttSubscribe
  * @note   解析MQTT订阅消息，该函数被外部调用。
  * @param  topic：订阅的主题字符串
  * @param  message：订阅的消息字符串
  * @retval true：解析成功
  * @retval false：解析失败
  */
bool mqttSubscribe(char *topic, char *message)
{
    /* 此函数用于解析订阅消息，根据不同的模式提取主题和消息内容，具体实现根据实际的消息格式进行编写。 */
    char *pcSubRecMsg = 0;
    pcSubRecMsg = strstr(wifiReceiver.buffer, message);
    if (strstr(wifiReceiver.buffer, topic), pcSubRecMsg)
        return true;
    return false;                                                       // 占位符，实际实现需要返回解析结果
}
