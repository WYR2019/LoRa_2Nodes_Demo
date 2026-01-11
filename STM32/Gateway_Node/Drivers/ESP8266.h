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

#define                 macESP8266_USART_BAUD_RATE          115200

#define                 macESP8266_USART                    USART2
#define                 macESP8266_USART_CLK                RCC_APB1Periph_USART2
#define                 macESP8266_USART_GPIO_CLK           RCC_APB2Periph_GPIOA
#define                 macESP8266_USART_TX_PORT            GPIOA
#define                 macESP8266_USART_TX_PIN             GPIO_Pin_2
#define                 macESP8266_USART_RX_PORT            GPIOA
#define                 macESP8266_USART_RX_PIN             GPIO_Pin_3

/* ESP8266 函数宏定义 */
// #define                 vEsp8266Printf(*format, ...)        vUsartPrintf(macESP8266_USART, *format, ...)
#define                 macESP8266_CH_ENABLE()              GPIO_SetBits ( macESP8266_CH_PD_PORT, macESP8266_CH_PD_PIN )
#define                 macESP8266_CH_DISABLE()             GPIO_ResetBits ( macESP8266_CH_PD_PORT, macESP8266_CH_PD_PIN )

#define                 macESP8266_RST_HIGH_LEVEL()         GPIO_SetBits ( macESP8266_RST_PORT, macESP8266_RST_PIN )
#define                 macESP8266_RST_LOW_LEVEL()          GPIO_ResetBits ( macESP8266_RST_PORT, macESP8266_RST_PIN )

/* ESP8266 函数声明 */
static void             vEsp8266GpioConfig                  ( void );
void                    vEsp8266Init                        ( void );
void                    vEsp8266Rst                         ( void );
bool                    bEsp8266Command                     ( char * pcCmd, char * pcAck1, char * pcAck2, uint32_t ulWaittime );
void                    vEsp8266AtTest                      ( void );
bool                    bEsp8266NetModeChoose               ( eNetMode_t xMode );
bool                    bEsp8266JoinAp                      ( char * pcSsid, char * pcPassWord );
bool                    bEsp8266MqttInit                    ( char * pcMqttUserName, char * pcMqttPassword, char * pcMqttClientId, char * pcMqttServerIp, uint16_t usMqttServerPort, char * pcMqttSubscribeTopic );
bool                    bEsp8266BuildAp                     ( char * pcSsid, char * pcPassWord, eApPsdMode_t xPsdMode );
bool                    bEsp8266EnableMultipleId            ( FunctionalState xEnumEnUnvarnishTx );
bool                    bEsp8266LinkServer                  ( eNetPro_t enumE, char * pcIp, char * pcComNum, eIdNo_t xId);
bool                    bEsp8266StartOrShutServer           ( FunctionalState xMode, char * pcPortNum, char * pcTimeOver );
uint8_t                 ucEsp8266GetLinkStatus              ( void );
uint8_t                 ucEsp8266GetIdLinkStatus            ( void );
uint8_t                 ucEsp8266InquireApIp                ( char * pcApIp, uint8_t ucArrayLength );
bool                    bEsp8266UnvarnishSend               ( void );
void                    vEsp8266ExitUnvarnishSend           ( void );
bool                    bEsp8266SendString                  ( FunctionalState xEnumEnUnvarnishTx, char * pcStr, uint32_t ulStrLength, eIdNo_t xId );
char *                  pcEsp8266ReceiveString              ( FunctionalState xEnumEnUnvarnishTx );

/* 用户需要设置的参数 */
#define                 ESP8266_APSSID                      "TESTAP888"                                                                                         // 要连接的热点的名称
#define                 ESP8266_APPWD                       "123345567"                                                                                         // 要连接的热点的密钥

#define                 ESP8266_MQTT_SERVER_IP              "121.36.104.9"                                                                                      // MQTT服务器IP地址
#define                 ESP8266_MQTT_SERVER_PORT            1883                                                                                                // MQTT服务器端口号
#define                 ESP8266_MQTT_CLIENT_ID              "LoRaEnvAssistant&stm32"                                                                            // MQTT客户端ID
#define                 ESP8266_MQTT_USERNAME               "stm32"                                                                                             // MQTT用户名
#define                 ESP8266_MQTT_PASSWORD               "123456789"                                                                                         // MQTT密码

#define                 ESP8266_ALIYUN_MQTT_IP              "iot-06z00by9al78fmz.mqtt.iothub.aliyuncs.com"
#define                 ESP8266_ALIYUN_MQTT_PORT            1883
#define                 ESP8266_ALIYUN_MQTT_CLIENT_ID       "k11ilqnf8mF.GATEWAY_STM32_ESP8266|securemode=2\\,signmethod=hmacsha256\\,timestamp=1768114890602|"          
#define                 ESP8266_ALIYUN_MQTT_USERNAME        "GATEWAY_STM32_ESP8266&k11ilqnf8mF"
#define                 ESP8266_ALIYUN_MQTT_PASSWORD        "8457286184495376a925f47ca284bb46a2fe8f6a6b76b0479ddb066038fe0d26"
#define                 ESP8266_ALIYUN_MQTT_SUBSCRIBE_TOPIC "/k11ilqnf8mF/GATEWAY_STM32_ESP8266/user/get"

/* 外部全局变量 */

#endif /* __ESP8266_H */
