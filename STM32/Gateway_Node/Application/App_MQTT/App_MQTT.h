#ifndef __APP_MQTT_H__
#define __APP_MQTT_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "ESP8266.h"

typedef enum
{
    /* data */
    MQTT_CMD_LED_ON = 0,
    MQTT_CMD_LED_OFF,
    MQTT_CMD_FAN_ON,
    MQTT_CMD_FAN_OFF,
    MQTT_CMD_HUMIDIFIER_ON,
    MQTT_CMD_HUMIDIFIER_OFF,
    MQTT_CMD_UNKNOWN = -1,
} eMqttCmd_t;

eMqttCmd_t xMqttCmdParse(char *pcTopic);

#endif /* __APP_MQTT_H__ */
