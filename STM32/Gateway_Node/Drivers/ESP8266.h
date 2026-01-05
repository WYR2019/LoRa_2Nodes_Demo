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
} ENUM_Net_ModeTypeDef;

typedef enum{
    enumTCP,
    enumUDP,
} ENUM_NetPro_TypeDef;

typedef enum{
    Multiple_ID_0 = 0,
    Multiple_ID_1,
    Multiple_ID_2,
    Multiple_ID_3,
    Multiple_ID_4,
    Single_ID_0,
} ENUM_ID_NO_TypeDef;

typedef enum{
    OPEN = 0,
    WEP,
    WPA_PSK,
    WPA2_PSK,
    WPA_WPA2_PSK,
} ENUM_AP_PsdMode_TypeDef;

/* ESP8266 外部全局变量声明 */
#define                 RX_BUF_MAX_LEN                      1024                                                                                                // 最大接收缓存字节数

extern struct STRUCT_USARTx_Fram                                                                                                                                // 串口数据帧的处理结构体
{
    char  Data_RX_BUF [ RX_BUF_MAX_LEN ];
    union {
    __IO uint16_t InfAll;
    struct {
            __IO uint16_t FramLength       :15;                                                                                                                 // 14:0
            __IO uint16_t FramFinishFlag   :1;                                                                                                                  // 15
        } InfBit;
    }; 
} strEsp8266_Fram_Record;

extern volatile uint8_t ucTcpClosedFlag;

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
bool                    bEsp8266Command                     ( char * cCmd, char * cReply1, char * cReply2, uint32_t ulWaittime );
void                    vEsp8266AtTest                      ( void );
bool                    bEsp8266NetModeChoose               ( ENUM_Net_ModeTypeDef enumMode );
bool                    bEsp8266JoinAp                      ( char * pSSID, char * pPassWord );
bool                    bEsp8266BuildAp                     ( char * pSSID, char * pPassWord, ENUM_AP_PsdMode_TypeDef enunPsdMode );
bool                    bEsp8266EnableMultipleId            ( FunctionalState enumEnUnvarnishTx );
bool                    bEsp8266LinkServer                  ( ENUM_NetPro_TypeDef enumE, char * ip, char * ComNum, ENUM_ID_NO_TypeDef id);
bool                    bEsp8266StartOrShutServer           ( FunctionalState enumMode, char * pPortNum, char * pTimeOver );
uint8_t                 ucEsp8266GetLinkStatus              ( void );
uint8_t                 ucEsp8266GetIdLinkStatus            ( void );
uint8_t                 ucEsp8266InquireApIp                ( char * pApIp, uint8_t ucArrayLength );
bool                    bEsp8266UnvarnishSend               ( void );
void                    vEsp8266ExitUnvarnishSend           ( void );
bool                    bEsp8266SendString                  ( FunctionalState enumEnUnvarnishTx, char * pStr, uint32_t ulStrLength, ENUM_ID_NO_TypeDef ucId );
char *                  pcEsp8266ReceiveString              ( FunctionalState enumEnUnvarnishTx );

/* 用户需要设置的参数 */
#define                 ESP8266_APSSID                      "TESTAP666"                                                                                         // 要连接的热点的名称
#define                 ESP8266_APPWD                       "123345567"                                                                                         // 要连接的热点的密钥

/* 外部全局变量 */

#endif /* __ESP8266_H */
