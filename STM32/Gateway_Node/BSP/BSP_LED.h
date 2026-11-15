/* 添加一个防止头文件重复包含的代码if not define，如果没有定义该字符串，那么就先定义一个 */
#ifndef __LED_H__
#define __LED_H__

#include "stm32f10x.h"                               // Device header

#define LED_PC13_WORK_MODE                           LIGHT_MODE

#define STM32_STATE_MODE                             0
#define LIGHT_MODE                                   1

/* PC13LED的设置 */
#define LED_PC13_GPIO_CLOCK                          RCC_APB2Periph_GPIOC
#define LED_PC13_GPIO_PORT                           GPIOC
#define LED_PC13_GPIO_PIN                            GPIO_Pin_13

/* 变量声明 */
void pc13LedInit(void);
void pc13LedOn(void);
void pc13LedOff(void);
void pc13LedTurn(void);

#endif
