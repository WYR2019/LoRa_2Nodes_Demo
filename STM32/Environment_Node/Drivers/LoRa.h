#ifndef __LORA_H__
#define __LORA_H__

#include "USART3.h"

// #define LORA_MODE

/* LoRa的静态设置 */
#define LORA_MODE_INIT                                        1
#define LORA_GPIO_PIN_TX                                      GPIO_Pin_10
#define LORA_GPIO_PIN_RX                                      GPIO_Pin_11
#define LORA_GPIO_PIN_ATK_AUX                                 GPIO_Pin_4 //PA4
#define LORA_GPIO_PIN_ATK_MD0                                 GPIO_Pin_3 //PB3
#define LORA_GATE_ADDR_HIGH                                   0x03
#define LORA_GATE_ADDR_LOW                                    0xE9
#define LORA_GATE_CHANNEL                                     0x17
#define LORA_NODE1_ADDR_HIGH                                  0x03
#define LORA_NODE1_ADDR_LOW                                   0xEA
#define LORA_NODE1_CHANNEL                                    0x17
#define LORA_NODE1_IDENTIFIER                                 0xD1
#define LORA_NODE2_ADDR_HIGH                                  0x03
#define LORA_NODE2_ADDR_LOW                                   0xEB
#define LORA_NODE2_CHANNEL                                    0x17
#define LORA_NODE2_IDENTIFIER                                 0xD2
#define LORA_SENSOR_DHT11                                     0xEA
#define LORA_SENSOR_MQ2                                       0xEB
#define LORA_SENSOR_LIGHT                                     0xEC
#define LORA_SENSOR_FIRE                                      0xED
#define LORA_EXECUTOR_LED                                     0xFA
#define LORA_EXECUTOR_FAN                                     0xFB
#define LORA_EXECUTOR_HUMIDIFIER                              0xFC
#define LORA_EXECUTOR_BUZZER                                  0xFD
#define LORA_EXECUTOR_SERVO                                   0xFE
#define LORA_EXECUTOR_STATUS_ON                               0x01
#define LORA_EXECUTOR_STATUS_OFF                              0x00

/* LoRa移植层 */

typedef enum {
    statusGatawayNode = 1,
    statusNode1,
    statusNode2,
    statusExeLed,
    statusExeFan,
    statusExeHumidifier,
    statusExeBuzzer,
    statusExeServo,
    statusExeOn,
    statusExeOff,
    statusLedOn,
    statusLedOff,
    statusFanOn,
    statusFanOff,
    statusHumidifierOn,
    statusHumidifierOff,
    statusBuzzerOn,
    statusBuzzerOff,
    statusServoOn,
    statusServoOff
}eLoRaMsgRecStatus;

typedef struct {
    uint8_t ucLoRaGateAddrHigh;
    uint8_t ucLoRaGateAddrLow;    
    uint8_t ucLoRaGateChannel;
} LoRaGateConfig_t;

typedef struct {
    uint8_t ucLoRaNode1AddrHigh;
    uint8_t ucLoRaNode1AddrLow;
    uint8_t ucLoRaNode1Channel;
    uint8_t ucLoRaNode1Identifier;
} LoRaNode1Config_t;

typedef struct {
    uint8_t ucLoRaNode2AddrHigh;
    uint8_t ucLoRaNode2AddrLow;
    uint8_t ucLoRaNode2Channel;
    uint8_t ucLoRaNode2Identifier;
} LoRaNode2Config_t;
	
typedef struct {
    uint8_t ucIdDht11;
    uint8_t ucIdMq2;
    uint8_t ucIdLight;
    uint8_t	ucIdFire;
} LoRaSensorID_t;

typedef struct {
    uint8_t ucIdLed;
    uint8_t ucIdFan;
    uint8_t ucIdHumidifier;
    uint8_t ucIdBuzzer;
    uint8_t ucIdServo;
} LoRaExecutorID_t;

typedef struct {
    uint8_t ucStatusOn;
    uint8_t ucStatusOff;
} LoRaExecutorFlag_t;

typedef struct {
    uint8_t ucRxState;                                        //状态变量S=0
    uint8_t ucRxReadIndex;                                      //指示接收到哪一个数据
    uint8_t ucRxNodeIndex;
    uint8_t ucRxExecutorIndex;
} LoRaUsart3Rx_t;

static LoRaGateConfig_t xLoRaGateConfig = {
    .ucLoRaGateAddrHigh    = LORA_GATE_ADDR_HIGH,
    .ucLoRaGateAddrLow     = LORA_GATE_ADDR_LOW,
    .ucLoRaGateChannel     = LORA_GATE_CHANNEL  
};

static LoRaNode1Config_t xLoRaNode1Config = {
    .ucLoRaNode1AddrHigh   = LORA_NODE1_ADDR_HIGH,
    .ucLoRaNode1AddrLow    = LORA_NODE1_ADDR_LOW,
    .ucLoRaNode1Channel    = LORA_NODE1_CHANNEL,
    .ucLoRaNode1Identifier = LORA_NODE1_IDENTIFIER,
};

static LoRaNode2Config_t xLoRaNode2Config = {
    .ucLoRaNode2AddrHigh   = LORA_NODE2_ADDR_HIGH,
    .ucLoRaNode2AddrLow    = LORA_NODE2_ADDR_LOW,
    .ucLoRaNode2Channel    = LORA_NODE2_CHANNEL,
    .ucLoRaNode2Identifier = LORA_NODE2_IDENTIFIER
};

static LoRaSensorID_t xLoRaSensorID = {
    .ucIdDht11      = LORA_SENSOR_DHT11,
    .ucIdMq2        = LORA_SENSOR_MQ2,
    .ucIdLight      = LORA_SENSOR_LIGHT,
    .ucIdFire       = LORA_SENSOR_FIRE
};

static LoRaExecutorID_t xLoRaExecutorID = {
    .ucIdLed        = LORA_EXECUTOR_LED,
    .ucIdFan        = LORA_EXECUTOR_FAN,
    .ucIdHumidifier = LORA_EXECUTOR_HUMIDIFIER,
    .ucIdBuzzer     = LORA_EXECUTOR_BUZZER,
    .ucIdServo      = LORA_EXECUTOR_SERVO
};

static LoRaExecutorFlag_t xLoRaExecutorStatus = {
    .ucStatusOn     = LORA_EXECUTOR_STATUS_ON,
    .ucStatusOff    = LORA_EXECUTOR_STATUS_OFF,
};

static LoRaUsart3Rx_t xLoRaUsart3Rx = { 0 };

void vLoRaConnectionPkt(uint8_t ucNodeId);
void vLoRaToGateIdPkt(uint8_t ucNodeId);
void vLoRaToGateSenIdPkt(uint8_t ucSensorId);
void vLoRaToGateExeIdPkt(uint8_t ucExecutorId);
eLoRaMsgRecStatus xLoRaMessageReceived(uint8_t *pucData);
eLoRaMsgRecStatus xLoRaMsgProcess(uint8_t ucDataRecNodeId, uint8_t ucDataRecExeId, uint8_t ucDataRecExeSta);


#endif
