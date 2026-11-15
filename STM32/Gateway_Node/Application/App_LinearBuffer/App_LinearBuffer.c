#include "App_LinearBuffer.h"

void initBuffer(LinearBuffer_t *serialReceiver)
{
    memset(serialReceiver->buffer, 0, sizeof(serialReceiver->buffer));
    serialReceiver->bufferIndex = 0;
}

void writeToBuffer(LinearBuffer_t *serialReceiver, uint8_t rxData)
{
    if (serialReceiver->bufferIndex < BUFFER_MAX_LENGTH)
    {
        serialReceiver->buffer[serialReceiver->bufferIndex++] = rxData;
    } else
    {
        serialReceiver->bufferIndex = 0;
        memset(serialReceiver->buffer, 0, sizeof(serialReceiver->buffer));
    }
}

void resetBufferIndex(LinearBuffer_t *serialReceiver)
{
    serialReceiver->bufferIndex = 0;
}
