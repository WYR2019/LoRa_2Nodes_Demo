#ifndef __LORA_CFG_H__
#define __LORA_CFG_H__

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
#define SENSORID_DHT11                                      0xB1
#define SENSORID_MQ2                                        0xB2
#define SENSORID_LIGHT                                      0xB3
#define SENSORID_FIRE                                       0xB4
#define EXECUTORID_LED                                      0xC1
#define EXECUTORID_FAN                                      0xC2
#define EXECUTORID_HUMIDIFIER                               0xC3
#define EXECUTORID_BUZZER                                   0xC4
#define EXECUTORID_SERVO                                    0xC5
#define LORA_EXECUTOR_COMMAND_ON                            0x01
#define LORA_EXECUTOR_COMMAND_OFF                           0x00

typedef struct
{
    union
    {
        uint8_t loRaGateAddrHigh;
        uint8_t loRaGateAddrLow;
        uint8_t loRaGateChannel;
    } GateCfg_t;
    union
    {
        uint8_t loRaNode1AddrHigh;
        uint8_t loRaNode1AddrLow;
        uint8_t loRaNode1Channel;
        uint8_t loRaNode1Identifier;
    } Node1Cfg_t;
    union
    {
        uint8_t loRaNode2AddrHigh;
        uint8_t loRaNode2AddrLow;
        uint8_t loRaNode2Channel;
        uint8_t loRaNode2Identifier;
    } Node2Cfg_t;
} LoRaNodeConfig_t;

static LoRaNodeConfig_t loRaNodeConfig = 
{
    .GateCfg_t = 
    {
        .loRaGateAddrHigh    = LORA_GATE_ADDR_HIGH,
        .loRaGateAddrLow     = LORA_GATE_ADDR_LOW,
        .loRaGateChannel     = LORA_GATE_CHANNEL
    },
    .Node1Cfg_t = 
    {
        .loRaNode1AddrHigh   = LORA_NODE1_ADDR_HIGH,
        .loRaNode1AddrLow    = LORA_NODE1_ADDR_LOW,
        .loRaNode1Channel    = LORA_NODE1_CHANNEL,
        .loRaNode1Identifier = LORA_NODE1_IDENTIFIER
    },
    .Node2Cfg_t = 
    {
        .loRaNode2AddrHigh   = LORA_NODE2_ADDR_HIGH,
        .loRaNode2AddrLow    = LORA_NODE2_ADDR_LOW,
        .loRaNode2Channel    = LORA_NODE2_CHANNEL,
        .loRaNode2Identifier = LORA_NODE2_IDENTIFIER
    }
};

typedef struct
{
    union
    {
        uint8_t dht11;
        uint8_t mq2;
        uint8_t light;
        uint8_t	fire;
    } SensorID_t;
    union
    {
        uint8_t led;
        uint8_t fan;
        uint8_t humidifier;
        uint8_t buzzer;
        uint8_t servo;
    } ExecutorId_t;
} LoRaRecvModuleId_t;

static LoRaRecvModuleId_t loRaRecvId = 
{
    .SensorID_t = 
    {
        .dht11      = SENSORID_DHT11,
        .mq2        = SENSORID_MQ2,
        .light      = SENSORID_LIGHT,
        .fire       = SENSORID_FIRE
    },
    .ExecutorId_t = 
    {
        .led        = EXECUTORID_LED,
        .fan        = EXECUTORID_FAN,
        .humidifier = EXECUTORID_HUMIDIFIER,
        .buzzer     = EXECUTORID_BUZZER,
        .servo      = EXECUTORID_SERVO
    },
};

#endif
