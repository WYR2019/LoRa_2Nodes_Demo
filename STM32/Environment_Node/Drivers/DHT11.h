#ifndef __DHT11_H__
#define __DHT11_H__

#include "stm32f10x.h"                  // Device header
#include "BSP_Delay.h"

/* DHT11的设置 */
#define DHT11_GPIO_CLK                                        RCC_APB2Periph_GPIOB
#define DHT11_GPIO_PORT                                       GPIOB
#define DHT11_GPIO_PIN                                        GPIO_Pin_1

/* 输出状态定义 */
#define DHT11_MODE_OUT                                        1
#define DHT11_MODE_IN                                         0
/* 控制DHT11引脚输出高低电平 */
#define DHT11_Low                                             GPIO_ResetBits(DHT11_GPIO_PORT, DHT11_GPIO_PIN)
#define DHT11_High                                            GPIO_SetBits(DHT11_GPIO_PORT, DHT11_GPIO_PIN)

typedef struct
{
    uint8_t temp;
    uint8_t humi;
} DHT11Data_t;

void dht11Mode(uint8_t mode);                               // DHT11引脚输出模式控制
void dht11Rst(void);                                        // 复位DHT11
uint8_t dht11Init(void);                                    // 初始化DHT11
uint8_t dht11Check(void);                                   // 检测DHT11
uint8_t dht11ReadBit(void);                                 // 读取一位的数据
uint8_t dht11ReadByte(void);                                // 读取一个字节的数据
uint8_t dht11ReadData(uint8_t *temp, uint8_t *humi);        // 读取温湿度数据

#endif
