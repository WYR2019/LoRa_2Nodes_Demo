#ifndef __ESP8266_SAMPLE_H__
#define	__ESP8266_SAMPLE_H__

/* ESP8266 函数宏定义 */

#define vEsp8266IrqHandler(void)            USART2_IRQHandler(void)
#define vEsp8266DataTransmit(format, ...)   vUsartPrintf(USART2, format, ##__VA_ARGS__)

#define ESP8266_CH_ENABLE()                 GPIO_SetBits(ESP8266_CH_PD_PORT, ESP8266_CH_PD_PIN)
#define ESP8266_CH_DISABLE()                GPIO_ResetBits(ESP8266_CH_PD_PORT, ESP8266_CH_PD_PIN)
#define ESP8266_RST_HIGH_LEVEL()            GPIO_SetBits(ESP8266_RST_PORT, ESP8266_RST_PIN)
#define ESP8266_RST_LOW_LEVEL()             GPIO_ResetBits(ESP8266_RST_PORT, ESP8266_RST_PIN)

#define ESP8266_APSSID                      "CMCC-5bjx"                                                                                                 // 要连接的热点的名称
#define ESP8266_APPWD                       "jp7qpqn3"                                                                                                  // 要连接的热点的密钥
#define ESP8266_ALIYUN_MQTT_IP              "iot-06z00by9al78fmz.mqtt.iothub.aliyuncs.com"
#define ESP8266_ALIYUN_MQTT_PORT            1883
#define ESP8266_ALIYUN_MQTT_PRODUCTKEY      "k11ilf46LJ0"
#define ESP8266_ALIYUN_MQTT_DEVICENAME      "stm32"
#define ESP8266_ALIYUN_MQTT_PASSWORD        "bd30fbe2b5595f0378594a66158a9e5938d0254ce84d8046627293881c6691a8"
#define ESP8266_ALIYUN_MQTT_CLIENTID        ESP8266_ALIYUN_MQTT_PRODUCTKEY"."ESP8266_ALIYUN_MQTT_DEVICENAME \
                                                            "|securemode=2\\,signmethod=hmacsha256\\,timestamp=1772730809576|"
#define ESP8266_ALIYUN_MQTT_USERNAME        ESP8266_ALIYUN_MQTT_DEVICENAME"&"ESP8266_ALIYUN_MQTT_PRODUCTKEY
#define ESP8266_ALIYUN_MQTT_SUBSCRIBE_TOPIC "/"ESP8266_ALIYUN_MQTT_PRODUCTKEY"/"ESP8266_ALIYUN_MQTT_DEVICENAME"/user/get"
#define ESP8266_ALIYUN_MQTT_PUBLISH_TOPIC   "/"ESP8266_ALIYUN_MQTT_PRODUCTKEY"/"ESP8266_ALIYUN_MQTT_DEVICENAME"/user/update" 

#include "stm32f10x.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "BSP_Delay.h"
#include "BSP_GPIO.h"
#include "BSP_USART.h"

/******************************* ESP8266 数据类型定义 ***************************/
typedef enum
{
    STA,
    AP,
    STA_AP
} eNetMode_t;

/* ESP8266 函数声明 */
void vEsp8266Rst(void);
bool bEsp8266Command(char *pcCmd, char *pcAck1, char *pcAck2, uint32_t ulWaittime);
void vEsp8266AtTest(void);
bool bEsp8266NetModeChoose(eNetMode_t xMode);
bool bEsp8266JoinAp(char *pcSsid, char *pcPassWord);
bool bEsp8266MqttInit(char *pcMqttUserName, char *pcMqttPassword, char *pcMqttClientId, char *pcMqttServerIp, uint16_t usMqttServerPort, char *pcMqttSubscribeTopic);
bool bMqttPublish(char *pcTopic, char *pcMessage);
bool bMqttSubscribe(char *pcTopic, char *pcMessage);

/* 外部全局变量 */

#endif /* __ESP8266_H */
