#ifndef __RELAY_H__
#define	__RELAY_H__

#include "stm32f10x.h"
#include <stdbool.h>

#define fanRelayInit(void)          pa0RelayInit(void)
#define fanOn(void)                 pa0RelayOn(void)
#define fanOff(void)                pa0RelayOff(void)

#define humidifierRelayInit(void)   pa1RelayInit(void)
#define humidifierOn(void)          pa1RelayOn(void)
#define humidifierOff(void)         pa1RelayOff(void)

#define	RELAY_PA0_CLOCK             RCC_APB2Periph_GPIOA
#define RELAY_PA0_GPIO_PIN          GPIO_Pin_0
#define RELAY_PA0_GPIO_PORT         GPIOA

#define	RELAY_PA1_CLOCK             RCC_APB2Periph_GPIOA
#define RELAY_PA1_GPIO_PIN          GPIO_Pin_1
#define RELAY_PA1_GPIO_PORT         GPIOA

void pa0RelayInit(void);
void pa0RelayOn(void);
void pa0RelayOff(void);
void pa1RelayInit(void);
void pa1RelayOn(void);
void pa1RelayOff(void);
bool getPa0RelayStatus(void);
bool getPa1RelayStatus(void);

#endif
