#include "CircularBuffer.h"

void initBuffer(SerialReceiver_t *serialReceiver)
{
    memset(serialReceiver->recvBuffer, 0, BUFFER_MAX_LENGTH - 1);
    serialReceiver->bufferIndex = 0;
}

void writeToBuffer(SerialReceiver_t *serialReceiver, uint8_t rxData)
{
    if (serialReceiver->bufferIndex < (BUFFER_MAX_LENGTH))                  // 预留1个字节写结束符
        serialReceiver->recvBuffer[serialReceiver->bufferIndex++] = rxData;
}

void resetBufferIndex(SerialReceiver_t *serialReceiver)
{
    serialReceiver->bufferIndex = 0;
}
