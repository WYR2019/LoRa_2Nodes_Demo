/* 添加一个防止头文件重复包含的代码if not define，如果没有定义该字符串，那么就先定义一个 */
#ifndef __LED_H__
#define __LED_H__

#include "stm32f10x.h"                              // Device header
#include <stdbool.h>

/* PC13LED的设置 */
#define LED_PC13_GPIO_CLOCK                         RCC_APB2Periph_GPIOC
#define LED_PC13_GPIO_PORT                          GPIOC
#define LED_PC13_GPIO_PIN                           GPIO_Pin_13

/* PB0LED的设置 */
#define LED_PB0_GPIO_CLOCK                          RCC_APB2Periph_GPIOB
#define LED_PB0_GPIO_PORT                           GPIOB
#define LED_PB0_GPIO_PIN                            GPIO_Pin_0

/* 变量声明 */
void pc13LedInit(void);
void pc13LedOn(void);
void pc13LedOff(void);
void pc13LedTurn(void);
bool getPc13LedStatus(void);
void pb0LedInit(void);
void pb0LedOn(void);
void pb0LedOff(void);
void pb0LedTurn(void);
bool getPb0LedStatus(void);

#endif
