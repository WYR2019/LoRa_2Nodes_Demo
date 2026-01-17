#ifndef __APP_MQTT_H__
#define __APP_MQTT_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "ESP8266.h"

typedef enum
{
    MQTT_MODE_NORMAL = 0,
    MQTT_MODE_PROPERTY,
}eMqttMode_t;

bool bMqttSubMsgFormat(eMqttMode_t ucMode, char *pcReceivedMsg, char *pcTopic, char *pcMessage);
bool bMqttDefaultSubscribe(char *pcTopic, char *pcMessage);
bool bMqttNormalPublish(char *pcTopic, char *pcMessage);

#endif /* __APP_MQTT_H__ */
