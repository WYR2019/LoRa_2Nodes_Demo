#ifndef __LORA_H__
#define __LORA_H__

#include <stdbool.h>
#include "BSP_Delay.h"
#include "BSP_GPIO.h"
#include "BSP_USART.h"

/* LoRa的静态设置 */
#define vLoRaIrqHandler(void)                               USART3_IRQHandler(void)
#define vLoRaDataTransmit(format, ...)                      vUsartPrintf(USART3, format, ##__VA_ARGS__)
#define vLoRaHexNumTransmit(pucArray, usLength)             vUsartSendArray(USART3, pucArray, usLength)
#define LORA_MD0_SETHIGHLVL()                               GPIO_SetBits(LORA_MD0_PORT, LORA_MD0_PIN)
#define LORA_MD0_SETLOWLVL()                                GPIO_ResetBits(LORA_MD0_PORT, LORA_MD0_PIN)

#define LORA_MAX_BUFFER                                     512

#define LORA_MSG_HEAD                                       0xAA
#define LORA_GATE_ADDR_HIGH                                 0x03
#define LORA_GATE_ADDR_LOW                                  0xE9
#define LORA_GATE_CHANNEL                                   0x17
#define LORA_NODE1_ADDR_HIGH                                0x03
#define LORA_NODE1_ADDR_LOW                                 0xEA
#define LORA_NODE1_CHANNEL                                  0x17
#define LORA_NODE2_ADDR_HIGH                                0x03
#define LORA_NODE2_ADDR_LOW                                 0xEB
#define LORA_NODE2_CHANNEL                                  0x17
#define LORA_NODE1_IDENTIFIER                               0xA1
#define LORA_NODE2_IDENTIFIER                               0xA2
#define LORA_SENSOR_DHT11                                   0xB1
#define LORA_SENSOR_MQ2                                     0xB2
#define LORA_SENSOR_LIGHT                                   0xB3
#define LORA_SENSOR_FIRE                                    0xB4
#define LORA_EXECUTOR_LED                                   0xC1
#define LORA_EXECUTOR_FAN                                   0xC2
#define LORA_EXECUTOR_HUMIDIFIER                            0xC3
#define LORA_EXECUTOR_BUZZER                                0xC4
#define LORA_EXECUTOR_SERVO                                 0xC5
#define LORA_EXECUTOR_COMMAND_ON                            0x01
#define LORA_EXECUTOR_COMMAND_OFF                           0x00

typedef struct 
{
    union 
    {
        uint8_t ucLoRaGateAddrHigh;
        uint8_t ucLoRaGateAddrLow;    
        uint8_t ucLoRaGateChannel;
    } GateCfg_t;
    union
    {
        uint8_t ucLoRaNode1AddrHigh;
        uint8_t ucLoRaNode1AddrLow;
        uint8_t ucLoRaNode1Channel;
        uint8_t ucLoRaNode1Identifier;
    } Node1Cfg_t;
    union
    {
        uint8_t ucLoRaNode2AddrHigh;
        uint8_t ucLoRaNode2AddrLow;
        uint8_t ucLoRaNode2Channel;
        uint8_t ucLoRaNode2Identifier;
    } Node2Cfg_t;
} LoRaNodeConfig_t;

typedef struct
{
    union
    {
        uint8_t ucIdDht11;
        uint8_t ucIdMq2;
        uint8_t ucIdLight;
        uint8_t	ucIdFire;    
    } SensorID_t;
    union
    {
        uint8_t ucIdLed;
        uint8_t ucIdFan;
        uint8_t ucIdHumidifier;
        uint8_t ucIdBuzzer;
        uint8_t ucIdServo;
    } ExecutorId_t;
} LoRaMsgRecvId_t;

typedef struct
{
    struct
    {
        uint8_t ucTemperature;
        uint8_t ucHumidity;
    } Sensor_t;
    struct
    {
        uint8_t ucLightStatus;
        uint8_t ucFanStatus;
        uint8_t ucHumidifierStatus;
    } Executor_t;
} LoRaMsgRecvData_t;


static LoRaNodeConfig_t xLoRaNodeConfig = 
{
    .GateCfg_t = 
    {
        .ucLoRaGateAddrHigh    = LORA_GATE_ADDR_HIGH,
        .ucLoRaGateAddrLow     = LORA_GATE_ADDR_LOW,
        .ucLoRaGateChannel     = LORA_GATE_CHANNEL
    },
    .Node1Cfg_t = 
    {
        .ucLoRaNode1AddrHigh   = LORA_NODE1_ADDR_HIGH,
        .ucLoRaNode1AddrLow    = LORA_NODE1_ADDR_LOW,
        .ucLoRaNode1Channel    = LORA_NODE1_CHANNEL,
        .ucLoRaNode1Identifier = LORA_NODE1_IDENTIFIER
    },
    .Node2Cfg_t = 
    {
        .ucLoRaNode2AddrHigh   = LORA_NODE2_ADDR_HIGH,
        .ucLoRaNode2AddrLow    = LORA_NODE2_ADDR_LOW,
        .ucLoRaNode2Channel    = LORA_NODE2_CHANNEL,
        .ucLoRaNode2Identifier = LORA_NODE2_IDENTIFIER
    }
};

static LoRaMsgRecvId_t xLoRaMsgRecvId = 
{
    .SensorID_t = 
    {
        .ucIdDht11      = LORA_SENSOR_DHT11,
        .ucIdMq2        = LORA_SENSOR_MQ2,
        .ucIdLight      = LORA_SENSOR_LIGHT,
        .ucIdFire       = LORA_SENSOR_FIRE
    },
    .ExecutorId_t = 
    {
        .ucIdLed        = LORA_EXECUTOR_LED,
        .ucIdFan        = LORA_EXECUTOR_FAN,
        .ucIdHumidifier = LORA_EXECUTOR_HUMIDIFIER,
        .ucIdBuzzer     = LORA_EXECUTOR_BUZZER,
        .ucIdServo      = LORA_EXECUTOR_SERVO
    },
};

bool bLoRaSendATCmdForAck(char *pcCmd, char *pcAck1, char *pcAck2, uint32_t ulWaittime);
void vLoRaConfigQuery(void);
void vLoRaConnection(uint8_t ucNodeId);
void ucLoRaRecvDataParse(uint8_t *pucBuffer, uint8_t ucLength);
void vLoRaParseDataTest(void);

#endif
