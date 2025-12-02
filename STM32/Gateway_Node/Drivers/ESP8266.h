#ifndef __ESP8266_H__
#define __ESP8266_H__

#include "string.h"
#include "stdbool.h"
#include "USART.h"
#include "NetFIFO.h"

#define ESP8266_MODE_STATION            0
#define ESP8266_MODE_SOFTAP             1

void vEsp8266Restore(void);
void vEsp8266Reset(void);
uint8_t ucEsp8266SetMode(uint8_t ucMode);
void vEsp8266JoinAp(const char *pcSsid, const char *pcPassword);
uint8_t ucEsp8266ResponseHandler(NetFifoBuffer_t *pxEspBuffer);

#endif
