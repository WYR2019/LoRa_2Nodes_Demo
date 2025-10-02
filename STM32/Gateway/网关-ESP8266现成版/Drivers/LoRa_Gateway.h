#ifndef __LORA_GATEWAY_H__
#define __LORA_GATEWAY_H__

#include "stm32f10x.h"                  															   // Device header
#include <stdio.h>
#include <stdarg.h>

/* LoRa的静态设置 */
#define LORA_MODE_INIT                                        1
#define LORA_GPIO_PIN_TX                                      GPIO_Pin_10
#define LORA_GPIO_PIN_RX                                      GPIO_Pin_11
#define LORA_GPIO_PIN_ATK_AUX                                 GPIO_Pin_4 //PA4
#define LORA_GPIO_PIN_ATK_MD0                                 GPIO_Pin_3 //PB3
#define LORA_NODE1_ADDR_HIGH                                  0x03
#define LORA_NODE1_ADDR_LOW                                   0xEA
#define LORA_NODE1_ADDR                                       ((LORA_NODE1_ADDR_HIGH << 8) | LORA_NODE1_ADDR_LOW)
#define LORA_NODE1_CHANNEL                                    0x17
#define LORA_NODE1_IDENTIFIER                                 0xD1
#define LORA_NODE2_ADDR_HIGH                                  0x03
#define LORA_NODE2_ADDR_LOW                                   0xEB
#define LORA_NODE2_ADDR                                       ((LORA_NODE2_ADDR_HIGH << 8) | LORA_NODE2_ADDR_LOW)
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

#if LORA_MODE_INIT
  typedef struct {
    uint8_t ucLoRaNode1Addr;
    uint8_t ucLoRaNode1Channel;
    uint8_t ucLoRaNode1Identifier;
    uint8_t ucLoRaNode2Addr;
    uint8_t ucLoRaNode2Channel;
    uint8_t ucLoRaNode2Identifier;
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

  typedef struct {
    int iTemp;
    int iHumi;
    int iFan;
    int iHumidifier;
    int iSmoke;
    int iFire;
    int iBeep;
    int iServo;
  } LoRaRxJsonFlag_t;

  static LoRaConfig_t xLoRaConfig = {
    .ucLoRaNode1Addr       = (uint8_t) LORA_NODE1_ADDR,
    .ucLoRaNode1Channel    = LORA_NODE1_CHANNEL,
    .ucLoRaNode1Identifier = LORA_NODE1_IDENTIFIER,
    .ucLoRaNode2Addr       = (uint8_t) LORA_NODE2_ADDR,
    .ucLoRaNode2Channel    = LORA_NODE2_CHANNEL,
    .ucLoRaNode2Identifier = LORA_NODE2_IDENTIFIER
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

  static LoRaRxJsonFlag_t xLoRaRxJsonFlag = {
    .iTemp = 0,
    .iHumi = 0,
    .iFan = 0,
    .iHumidifier = 0,
    .iSmoke = 0,
    .iFire = 0,
    .iBeep = 0,
    .iServo = 0
  };

  extern uint8_t loRaNodeID, sensorID;
  extern uint8_t loRaUSART3RxPacket[80];

  void vLoRaUSART3EnableInit(uint32_t ulLoRaUSART3Baudrate);
  void vLoRaUSART3SendArray(uint8_t *pucArray,uint16_t usLength);
  void vLoRaUSART3Node1IDPkt(void);
  void vLoRaUSART3Node2IDPkt(void);
  void LoRa_USART3_Node1_Fan_On_Msg(void);
  void LoRa_USART3_Node1_Fan_Off_Msg(void);
  void LoRa_USART3_Node1_Humidifier_On_Msg(void);
  void LoRa_USART3_Node1_Humidifier_Off_Msg(void);

  
#else
  void LoRa_USART3_SendString(char *string);
#endif
#endif
