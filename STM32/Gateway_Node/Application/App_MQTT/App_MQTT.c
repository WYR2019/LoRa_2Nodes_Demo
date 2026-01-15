#include "App_MQTT.h"

bool bMqttNormalSubscribe(char *pcTopic, char *pcMessage)
{
    char cCmd[256] = {0};
    snprintf(cCmd, sizeof(cCmd), "AT+MQTT_SUBSCRIBE=\"%s\",\"%s\"", pcTopic, pcMessage);
    // Add logic to send the command and handle the response
    return true;
}
