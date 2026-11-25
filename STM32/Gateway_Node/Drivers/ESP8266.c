#include "ESP8266.h"

/* 初始化串口缓冲区句柄 */
circular_buffer_t wifi_receiver = {0};

/**
  * @brief  串口2接收中断处理函数
  * @note   接收串口中断发送的每帧数据到串口消息队列中，以便后续处理数据。
  * @param  None
  * @retval None
  */
void esp8266IrqHandler(void)
{
    uint8_t rx_data = 0;
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        rx_data = USART_ReceiveData(USART2);
        write_to_buffer(&wifi_receiver, rx_data);
        traceFromIrq(rx_data);                                                                   // 打印到指定串口调试
    }
    if (USART_GetITStatus(USART2, USART_IT_IDLE) == SET)                                        // 数据帧接收完毕
    {
        rx_data = USART_ReceiveData(USART2);                                                     // 由软件序列清除中断标志位(先读USART_SR，然后读USART_DR)
        write_to_buffer(&wifi_receiver, '\0');
    }
}

/**
 * @brief  isATCommandExists
 * @note   对ESP8266模块发送AT指令,并等待响应。该函数被外部调用。
 * @param  cmd：待发送的指令
 * @param  ack_1：期待的响应1，为NULL表不需响应
 * @param  ack_2：期待的响应2，为NULL表不需响应，两者为或逻辑关系。
 * @param  wait_time：等待响应的时间
 * @retval true：指令发送成功
 * @retval false：指令发送失败
 */
bool isATCommandExists(char *cmd, char *ack_1, char *ack_2, uint32_t wait_time)
{
    uint8_t transmit_string[256] = {0};
    snprintf((char *)transmit_string, sizeof(transmit_string), "%s\r\n", cmd);
    trace("AT Command: %s\r\n", transmit_string);
    esp8266DataTransmit(transmit_string);
    if ((ack_1 == 0) && (ack_2 == 0))                                                           // 不需要接收数据
        return true;
    delayMs(wait_time);                                                                         // 延时
    char data[1024] = {0};
    if ((ack_1 != 0) && (ack_2 != 0))
    {
        read_all_string_from_buffer(&wifi_receiver, data);
        return ((bool)strstr(data, ack_1) && (bool)strstr(data, ack_2));
        // if ((!(bool)strstr(data, ack_1) && (bool)strstr(data, ack_2)))                          // 重试
        // {
        //     char retry[64] = {0};
        //     read_all_string_from_buffer(&wifi_receiver, retry);
        //     return ((bool)strstr(retry, ack_1) && (bool)strstr(retry, ack_2));
        // }
    }
    else if (ack_1 != 0)
    {
        read_all_string_from_buffer(&wifi_receiver, data);
        return ((bool)strstr(data, ack_1));
    }
    else
    {
        read_all_string_from_buffer(&wifi_receiver, data);
        return ((bool)strstr(data, ack_2));
    }
    return false;
}

/**
  * @brief  esp8266Reset
  * @note   调用这个函数，就可以重启ESP8266模块，该函数被esp8266AtTest调用。
  * @param  无
  * @retval 无
  */
void esp8266Reset()
{
#if ESP8266_RST_MODE
    isATCommandExists("AT+RST", "OK", "ready", 2500);
#else
    ESP8266_RST_LOW_LEVEL();
    delayMs(500);
    ESP8266_RST_HIGH_LEVEL();
#endif
}

/**
  * @brief  esp8266AtTest
  * @note   对ESP8266模块进行AT测试启动，该函数被外部调用。
  * @param  无
  * @retval 无
  */
void esp8266AtTest(void)
{
    ESP8266_RST_HIGH_LEVEL();
    delayMs(1000);
    if (isATCommandExists("AT", "OK", NULL, 500) != true)
        return;
    esp8266Reset();
}

/** 
  * @brief  esp8266NetModeCheck
  * @note   检查ESP8266模块的工作模式,该函数被外部调用。
  * @param  mode，工作模式
  * @retval 1，检查成功
  * @retval 0，检查失败
  */
bool esp8266NetModeCheck(NetMode_t mode)
{
    /* 查询现有模式是否已设置完成，若没有则重新设置模式 */
    switch (mode)
    {
    case STA:
        if (isATCommandExists("AT+CWMODE?", "+CWMODE:1", NULL, 2500) == true)
        {
            return true;
        }
        return isATCommandExists("AT+CWMODE=1", "OK", NULL, 2500);
    case AP:
        if (isATCommandExists("AT+CWMODE?", "+CWMODE:2", NULL, 2500) == true)
        {
            return true;
        }
        return isATCommandExists("AT+CWMODE=2", "OK", NULL, 2500);
    case STA_AP:
        if (isATCommandExists("AT+CWMODE?", "+CWMODE:3", NULL, 2500) == true)
        {
            return true;
        }
        return isATCommandExists("AT+CWMODE=3", "OK", NULL, 2500);
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
    char cmd[64] = {0};
    sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"", ssid, password);
    return isATCommandExists(cmd, "WIFI CONNECTED", NULL, 5000);
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
    if (isATCommandExists(cmd, "OK", NULL, 2500) != true)
    {
        trace("Failed to configure MQTT user information.\r\n");
        return false;
    }

    /* 2. 连接到MQTT服务器 */
    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "AT+MQTTCONN=0,\"%s\",%u,1", 
        mqttServerIp, mqttServerPort);
    if (isATCommandExists(cmd, "OK", NULL, 8000) != true && 
        isATCommandExists(cmd, "+MQTTCONN", NULL, 8000) != true)
    {
        trace("Failed to connect to MQTT server.\r\n");
        return false;
    }

    /* 3. 订阅主题 */
    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "AT+MQTTSUB=0,\"%s\",1", mqttSubscribeTopic);
    if (isATCommandExists(cmd, "OK", NULL, 2500) != true)
    {
        trace("Failed to subscribe to MQTT topic.\r\n");
        return false;
    }

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
    return isATCommandExists(cmd, ">", NULL, 2500)
        && isATCommandExists(message, "OK", NULL, 2500);
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
    char *subRecMsg = 0;
    subRecMsg = strstr(wifi_receiver.buffer, message);
    if (strstr(wifi_receiver.buffer, topic), subRecMsg)
        return true;
    return false;                                                                               // 占位符，实际实现需要返回解析结果
}
