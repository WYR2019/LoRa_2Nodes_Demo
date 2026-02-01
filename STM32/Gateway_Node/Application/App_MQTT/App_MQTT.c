#include "App_MQTT.h"

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
    char cCmd[256] = {0};
    uint8_t ucLen = strlen(pcMessage);
    snprintf(cCmd, sizeof(cCmd), "AT+MQTTPUBRAW=0,\"%s\",%d,1,0", pcTopic, ucLen);
    return bEsp8266Command(cCmd, "OK", ">", 3000) && bEsp8266Command(pcMessage, "OK", NULL, 3000);
}

/**
  * @brief  bMqttSubMsgFormat
  * @note   解析MQTT订阅消息，该函数被外部调用。
  * @param  pcTopic：订阅的主题字符串
  * @param  pcMessage：订阅的消息字符串
  * @retval true：解析成功
  * @retval false：解析失败
  */
bool bMqttSubMsgFormat(char *pcTopic, char *pcMessage)
{
    // 此函数用于解析订阅消息，根据不同的模式提取主题和消息内容
    // 具体实现根据实际的消息格式进行编写
    char *pcSubRecMsg;
    pcSubRecMsg = strstr(xSerialFrameRecord.cSerialReceivedBuffer, pcMessage);
    if (strstr(xSerialFrameRecord.cSerialReceivedBuffer, pcTopic), pcSubRecMsg)
    {
        /* code */
        vUsartPrintf(USART1, "MQTT Subscribe Message Matched: %s\r\n", pcSubRecMsg);
        return true;
    }
    return false; // 占位符，实际实现需要返回解析结果
}

/**
  * @brief  xMqttCmdParse
  * @note   解析MQTT命令，该函数被外部调用。
  * @param  pcTopic：订阅的主题字符串
  * @retval eMqttCmd_t：解析得到的命令枚举值
  */
eMqttCmd_t xMqttCmdParse(char *pcTopic)
{
    if (bMqttSubMsgFormat(pcTopic, "{\"cmd\":{\"led\":\"on\"}}") == true)
    {
        return MQTT_CMD_LED_ON;
    }
    else if (bMqttSubMsgFormat(pcTopic, "{\"cmd\":{\"led\":\"off\"}}") == true)
    {
        return MQTT_CMD_LED_OFF;
    }
    else if (bMqttSubMsgFormat(pcTopic, "{\"cmd\":{\"fan\":\"on\"}}") == true)
    {
        return MQTT_CMD_FAN_ON;
    }
    else if (bMqttSubMsgFormat(pcTopic, "{\"cmd\":{\"fan\":\"off\"}}") == true)
    {
        return MQTT_CMD_FAN_OFF;
    }
    else if (bMqttSubMsgFormat(pcTopic, "{\"cmd\":{\"humidifier\":\"on\"}}") == true)
    {
        return MQTT_CMD_HUMIDIFIER_ON;
    }
    else if (bMqttSubMsgFormat(pcTopic, "{\"cmd\":{\"humidifier\":\"off\"}}") == true)
    {
        return MQTT_CMD_HUMIDIFIER_OFF;
    }
    return MQTT_CMD_UNKNOWN; // 未识别的命令
}
