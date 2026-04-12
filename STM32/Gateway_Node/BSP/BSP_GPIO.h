#ifndef __BSP_GPIO_H__
#define __BSP_GPIO_H__

#define ESP8266_CH_PD_CLK                   RCC_APB2Periph_GPIOA
#define ESP8266_CH_PD_PORT                  GPIOA
#define ESP8266_CH_PD_PIN                   GPIO_Pin_5
#define ESP8266_RST_CLK                     RCC_APB2Periph_GPIOA
#define ESP8266_RST_PORT                    GPIOA
#define ESP8266_RST_PIN                     GPIO_Pin_6
#define LORA_MD0_CLK                        RCC_APB2Periph_GPIOA
#define LORA_MD0_PORT                       GPIOA
#define LORA_MD0_PIN                        GPIO_Pin_4

#include "stm32f10x.h"                  // Device header

void vEsp8266GpioConfig(void);
void vLoRaGpioConfig(void);

#endif
