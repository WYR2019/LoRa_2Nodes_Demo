#ifndef __NETFIFO_H__
#define __NETFIFO_H__                                  1

#include "stm32f10x.h"
#include "stdio.h"
#include "string.h"
#include "stdbool.h"
#include "stdlib.h"

typedef struct
{
    char cNetFifoBuffer[1024];
    uint8_t ucBufferIndex;
}NetFifoBuffer_t;

void vNetBufferInit(NetFifoBuffer_t *pxBuffer);
void vNetBufferWrite(NetFifoBuffer_t *pxBuffer, char cData);
char cNetBufferReadByte(NetFifoBuffer_t *pxBuffer, int index);
bool bNetBufferReadString(NetFifoBuffer_t *pxBuffer, const char *pcStr);
int iNetBufferGetLength(NetFifoBuffer_t *pxBuffer);
void vNetBufferClear(NetFifoBuffer_t *pxBuffer);

#endif
