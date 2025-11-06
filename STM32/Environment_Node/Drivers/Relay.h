#ifndef __RELAY_H__
#define	__RELAY_H__

#include "stm32f10x.h"

#define vFanRelayInit(void)       vPa0RelayInit(void)
#define vFanOn(void)              vPa0RelayOn(void)
#define vFanOff(void)             vPa0RelayOff(void)

#define	PA0_RELAY_CLK             RCC_APB2Periph_GPIOA
#define PA0_RELAY_GPIO_PIN        GPIO_Pin_0
#define PA0_RELAY_GPIO_PORT       GPIOA

void vPa0RelayInit(void);
void vPa0RelayOn(void);
void vPa0RelayOff(void);

#endif
