#ifndef __LORA_H__
#define __LORA_H__
#include "stm32f10x.h"                                                   // Device header
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

/* LoRa的静态设置 */
#define LORA_MODE_INIT                                        1
#define LORA_GPIO_PIN_TX                                      GPIO_Pin_10
#define LORA_GPIO_PIN_RX                                      GPIO_Pin_11
#define LORA_GPIO_PIN_ATK_AUX                                 GPIO_Pin_4 //PA4
#define LORA_GPIO_PIN_ATK_MD0                                 GPIO_Pin_3 //PB3
#define LORA_GATE_ADDR_HIGH                                   0x03
#define LORA_GATE_ADDR_LOW                                    0xE9
#define LORA_GATE_CHANNEL                                     0x17
#define LORA_NODE_IDENTIFIER                                  0xD1
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
#define LORA_GATE_ADDR                                        ((LORA_GATE_ADDR_HIGH << 8) | LORA_GATE_ADDR_LOW)

#if LORA_MODE_INIT

  typedef struct {
    uint8_t ucLoRaAddr;
    uint8_t ucLoRaChannel;
    uint8_t ucLoRaGateAddr;
    uint8_t ucLoRaGateChannel;
    uint8_t ucLoRaIdentifier;
  } LoRaConfig_t;

  typedef struct {
    uint8_t ucDHT11;
    uint8_t ucMQ2;
    uint8_t ucLight;
    uint8_t	ucFire;
  } LoRaSensorID_t;

  typedef struct {
    uint8_t ucLED;
    uint8_t ucFan;
    uint8_t ucHumidifier;
    uint8_t ucBuzzer;
    uint8_t ucServo;
  } LoRaExecutorID_t;

  typedef struct {
    uint8_t ucState;
    uint8_t ucId;
    uint8_t ucStatusOn;
    uint8_t ucStatusOff;
  } LoRaExecutorFlag_t;

  typedef struct {
    uint8_t ucRxState;                                        //状态变量S=0
    uint8_t ucPRxPacket;                                      //指示接收到哪一个数据
  } LoRaUSART3Rx_t;

  static LoRaConfig_t xLoRaConfig = {
    .ucLoRaGateAddr    = (uint8_t) LORA_GATE_ADDR,
    .ucLoRaGateChannel = LORA_GATE_CHANNEL,
    .ucLoRaIdentifier  = LORA_NODE_IDENTIFIER
  };

  static LoRaSensorID_t xLoRaSensorID = {
    .ucDHT11      = LORA_SENSOR_DHT11,
    .ucMQ2        = LORA_SENSOR_MQ2,
    .ucLight      = LORA_SENSOR_LIGHT,
    .ucFire       = LORA_SENSOR_FIRE
  };

  static LoRaExecutorID_t xLoRaExecutorID = {
    .ucLED        = LORA_EXECUTOR_LED,
    .ucFan        = LORA_EXECUTOR_FAN,
    .ucHumidifier = LORA_EXECUTOR_HUMIDIFIER,
    .ucBuzzer     = LORA_EXECUTOR_BUZZER,
    .ucServo      = LORA_EXECUTOR_SERVO
  };

  static LoRaExecutorFlag_t xLoRaExecutorStatus = {
    .ucStatusOn   = LORA_EXECUTOR_STATUS_ON,
    .ucStatusOff  = LORA_EXECUTOR_STATUS_OFF,
  };

  static LoRaUSART3Rx_t xLoRaUSART3Rx = {
    .ucRxState = 0,
    .ucPRxPacket = 0,
  };

  void vLoRaUSART3EnableInit(uint32_t ulLoRaUSART3Baudrate);
  void vLoRaUSART3SendArray(uint8_t *pucArray,uint16_t usLength);

  void vLoRaUSART3GateIdPkt(void);
  void vLoRaNode1SendMsg(void);
  void vLoRaNode1Executing(void);

#else
  void vLoRaUSART3SendString(char *pcString);
  void vLoRaUSART3Printf(char *format, ...);
#endif
#endif
