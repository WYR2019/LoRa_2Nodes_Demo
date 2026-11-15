#ifndef __CIRCULAR_BUFFER_H__
#define __CIRCULAR_BUFFER_H__

#include "stm32f10x.h"
#include "string.h"

#define BUFFER_MAX_LENGTH                                               512     // 接收缓冲区最大字节数

typedef struct                                                                  // 串口数据帧的处理结构体
{
    char recvBuffer[BUFFER_MAX_LENGTH];
    volatile uint16_t bufferIndex                                       :9;     // 位域设计，uint16_t的底层存储单元通常为16位，:9占用低9位，从bit0-bit8，bit9-bit15不用，
                                                                                // 因此能表示的范围是0~512，原先配合rxflag的高一位bit15，共同占用一个内存，以达到节省内存的目的
}SerialReceiver_t;

void initBuffer(SerialReceiver_t *serialReceiver);
void writeToBuffer(SerialReceiver_t *serialReceiver, uint8_t rxData);
void resetBufferIndex(SerialReceiver_t *serialReceiver);

#endif
