#include "App_MQTT.h"

/**
  * @brief  xMqttCmdParse
  * @note   解析MQTT命令，该函数被外部调用。
  * @param  topic：订阅的主题字符串
  * @retval eMqttCmd_t：解析得到的命令枚举值
  */
MqttCmd_t mqttCmdParse(char *topic)
{
    if (mqttSubscribe(topic, "{\"cmd\":{\"led\":\"on\"}}") == true)
    {
        return MQTT_CMD_LED_ON;
    }
    else if (mqttSubscribe(topic, "{\"cmd\":{\"led\":\"off\"}}") == true)
    {
        return MQTT_CMD_LED_OFF;
    }
    else if (mqttSubscribe(topic, "{\"cmd\":{\"fan\":\"on\"}}") == true)
    {
        return MQTT_CMD_FAN_ON;
    }
    else if (mqttSubscribe(topic, "{\"cmd\":{\"fan\":\"off\"}}") == true)
    {
        return MQTT_CMD_FAN_OFF;
    }
    else if (mqttSubscribe(topic, "{\"cmd\":{\"humidifier\":\"on\"}}") == true)
    {
        return MQTT_CMD_HUMIDIFIER_ON;
    }
    else if (mqttSubscribe(topic, "{\"cmd\":{\"humidifier\":\"off\"}}") == true)
    {
        return MQTT_CMD_HUMIDIFIER_OFF;
    }
    return MQTT_CMD_UNKNOWN; // 未识别的命令
}


