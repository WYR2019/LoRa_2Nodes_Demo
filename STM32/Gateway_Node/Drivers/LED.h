/* 添加一个防止头文件重复包含的代码if not define，如果没有定义该字符串，那么就先定义一个 */
#ifndef __LED_H__
#define __LED_H__

#include "stm32f10x.h"                            // Device header

/* PC13LED的设置 */
#define LED_PC13_GPIO_CLOCK                                   RCC_APB2Periph_GPIOC
#define LED_PC13_GPIO_PORT                                    GPIOC
#define LED_PC13_GPIO_PIN                                     GPIO_Pin_13

/* 变量声明 */
void vPc13LedInit(void);
void vPc13LedOn(void);
void vPc13LedOff(void);
void vPc13LedTurn(void);

#endif
