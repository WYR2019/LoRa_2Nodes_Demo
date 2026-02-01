#ifndef __ESP8266_SAMPLE_H__
#define	__ESP8266_SAMPLE_H__

#include "stm32f10x.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "Delay.h"
#include "USART.h"

#if defined ( __CC_ARM )
#pragma anon_unions
#endif

/******************************* ESP8266 数据类型定义 ***************************/
typedef enum{
    STA,
    AP,
    STA_AP
} eNetMode_t;

typedef enum{
    ALIYUN = 0,
    EMQX
} eMqttServerMode_t;

typedef enum{
    enumTCP,
    enumUDP,
} eNetPro_t;

typedef enum{
    Multiple_ID_0 = 0,
    Multiple_ID_1,
    Multiple_ID_2,
    Multiple_ID_3,
    Multiple_ID_4,
    Single_ID_0,
} eIdNo_t;

typedef enum{
    OPEN = 0,
    WEP,
    WPA_PSK,
    WPA2_PSK,
    WPA_WPA2_PSK,
} eApPsdMode_t;

/* ESP8266 连接引脚定义 */
#define                 macESP8266_CH_PD_CLK                RCC_APB2Periph_GPIOA
#define                 macESP8266_CH_PD_PORT               GPIOA
#define                 macESP8266_CH_PD_PIN                GPIO_Pin_5
#define                 macESP8266_RST_CLK                  RCC_APB2Periph_GPIOA
#define                 macESP8266_RST_PORT                 GPIOA
#define                 macESP8266_RST_PIN                  GPIO_Pin_6

/* ESP8266 函数宏定义 */
// #define                 vEsp8266Printf(*format, ...)        vUsartPrintf(macESP8266_USART, *format, ...)
#define                 macESP8266_CH_ENABLE()              GPIO_SetBits ( macESP8266_CH_PD_PORT, macESP8266_CH_PD_PIN )
#define                 macESP8266_CH_DISABLE()             GPIO_ResetBits ( macESP8266_CH_PD_PORT, macESP8266_CH_PD_PIN )
#define                 macESP8266_RST_HIGH_LEVEL()         GPIO_SetBits ( macESP8266_RST_PORT, macESP8266_RST_PIN )
#define                 macESP8266_RST_LOW_LEVEL()          GPIO_ResetBits ( macESP8266_RST_PORT, macESP8266_RST_PIN )

/* 用户需要设置的参数 */
#define                 ESP8266_APSSID                      "CMCC-5bjx"                                                                                             // 要连接的热点的名称
#define                 ESP8266_APPWD                       "jp7qpqn3"                                                                                              // 要连接的热点的密钥

#define                 ALIYUN                              0
#define                 EMQX                                1
#define                 ESP8266_MQTT_SERVER_MODE            EMQX                                                                                                  // 选择MQTT服务器模式：ALIYUN/EMQX

#define                 ESP8266_EMQX_MQTT_SERVER_IP         "121.36.104.9"                                                                                          // MQTT服务器IP地址
#define                 ESP8266_EMQX_MQTT_SERVER_PORT       1883                                                                                                    // MQTT服务器端口号
#define                 ESP8266_EMQX_MQTT_PROJECTID         "LoRaEnvAssistant"                                                                                      // MQTT项目ID
#define                 ESP8266_EMQX_MQTT_DEVICENAME        "stm32"                                                                                                 // MQTT设备名称
#define                 ESP8266_EMQX_MQTT_SUB_DEVICENAME    "mqttfx"                                                                                                // MQTT订阅设备名称
#define                 ESP8266_EMQX_MQTT_CLIENTID          ESP8266_EMQX_MQTT_PROJECTID"&"ESP8266_EMQX_MQTT_DEVICENAME                                              // MQTT客户端ID
#define                 ESP8266_EMQX_MQTT_USERNAME          ESP8266_EMQX_MQTT_DEVICENAME                                                                            // MQTT用户名
#define                 ESP8266_EMQX_MQTT_PASSWORD          "123456789"                                                                                             // MQTT密码
#define                 ESP8266_EMQX_MQTT_SUBSCRIBE_TOPIC   "/"ESP8266_EMQX_MQTT_PROJECTID"/"ESP8266_EMQX_MQTT_PROJECTID"&"ESP8266_EMQX_MQTT_SUB_DEVICENAME"/topics"                                                  // 订阅主题
#define                 ESP8266_EMQX_MQTT_PUBLISH_TOPIC     "/"ESP8266_EMQX_MQTT_PROJECTID"/"ESP8266_EMQX_MQTT_CLIENTID"/topics"                                    // 发布主题

#define                 ESP8266_ALIYUN_MQTT_IP              "iot-06z00by9al78fmz.mqtt.iothub.aliyuncs.com"
#define                 ESP8266_ALIYUN_MQTT_PORT            1883
#define                 ESP8266_ALIYUN_MQTT_PRODUCTKEY      "k11ilqnf8mF"
#define                 ESP8266_ALIYUN_MQTT_DEVICENAME      "GATEWAY_STM32_ESP8266"
#define                 ESP8266_ALIYUN_MQTT_CLIENTID        ESP8266_ALIYUN_MQTT_PRODUCTKEY"."ESP8266_ALIYUN_MQTT_DEVICENAME \
                                                            "|securemode=2\\,signmethod=hmacsha256\\,timestamp=1768415322303|"
#define                 ESP8266_ALIYUN_MQTT_USERNAME        ESP8266_ALIYUN_MQTT_DEVICENAME"&"ESP8266_ALIYUN_MQTT_PRODUCTKEY
#define                 ESP8266_ALIYUN_MQTT_PASSWORD        "a74261b04d7dda672342776b88e956f26f74f4d9d104461f8b65cbf544ab9c03"
#define                 ESP8266_ALIYUN_MQTT_SUBSCRIBE_TOPIC "/"ESP8266_ALIYUN_MQTT_PRODUCTKEY"/"ESP8266_ALIYUN_MQTT_DEVICENAME"/user/get"
#define                 ESP8266_ALIYUN_MQTT_PUBLISH_TOPIC   "/"ESP8266_ALIYUN_MQTT_PRODUCTKEY"/"ESP8266_ALIYUN_MQTT_DEVICENAME"/user/update"

/* ESP8266 函数声明 */
void                    vEsp8266GpioConfig                  ( void );
void                    vEsp8266Rst                         ( void );
bool                    bEsp8266Command                     ( char * pcCmd, char * pcAck1, 
                                                              char * pcAck2, uint32_t ulWaittime );
void                    vEsp8266AtTest                      ( void );
bool                    bEsp8266NetModeChoose               ( eNetMode_t xMode );
bool                    bEsp8266JoinAp                      ( char * pcSsid, char * pcPassWord );
bool                    bEsp8266MqttInit                    ( char * pcMqttUserName, char * pcMqttPassword, char * pcMqttClientId, 
                                                              char * pcMqttServerIp, uint16_t usMqttServerPort, char * pcMqttSubscribeTopic );
bool                    bEsp8266BuildAp                     ( char * pcSsid, char * pcPassWord, eApPsdMode_t xPsdMode );
bool                    bEsp8266EnableMultipleId            ( FunctionalState xEnumEnUnvarnishTx );
bool                    bEsp8266LinkServer                  ( eNetPro_t xNetProtocol, char * pcIp, 
                                                              char * pcComNum, eIdNo_t xId);
bool                    bEsp8266StartOrShutServer           ( FunctionalState xMode, char * pcPortNum, char * pcTimeOver );
uint8_t                 ucEsp8266GetLinkStatus              ( void );
uint8_t                 ucEsp8266GetIdLinkStatus            ( void );
uint8_t                 ucEsp8266InquireApIp                ( char * pcApIp, uint8_t ucArrayLength );
bool                    bEsp8266UnvarnishSend               ( void );
void                    vEsp8266ExitUnvarnishSend           ( void );
bool                    bEsp8266SendString                  ( FunctionalState xEnumEnUnvarnishTx, char * pcStr, 
                                                              uint32_t ulStrLength, eIdNo_t xId );
char *                  pcEsp8266ReceiveString              ( FunctionalState xEnumEnUnvarnishTx );

/* 外部全局变量 */

#endif /* __ESP8266_H */
