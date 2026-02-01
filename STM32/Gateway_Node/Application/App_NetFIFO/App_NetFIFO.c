#include "App_NetFIFO.h"

/**
  * @brief  初始化环形缓冲区
  * @param  pxBuffer: 缓冲区结构体指针
  * @retval None
  */
void vNetBufferInit(NetFifoBuffer_t *pxBuffer)
{
    if (pxBuffer == NULL) return;
    memset(pxBuffer->cNetFifoBuffer, 0, sizeof(pxBuffer->cNetFifoBuffer));
    pxBuffer->ucBufferIndex = 0;
}

/**
  * @brief  向缓冲区写入数据
  * @param  pxBuffer: 缓冲区结构体指针
  * @param  cData:    要写入的数据
  * @retval None
  */
void vNetBufferWrite(NetFifoBuffer_t *pxBuffer, char cData)
{
    if (pxBuffer == NULL) return;
    if (pxBuffer->ucBufferIndex < sizeof(pxBuffer->cNetFifoBuffer) - 1)
    {
        pxBuffer->cNetFifoBuffer[pxBuffer->ucBufferIndex++] = cData;
        pxBuffer->cNetFifoBuffer[pxBuffer->ucBufferIndex] = '\0'; // 确保字符串终止
    }
}

/**
  * @brief  从缓冲区读取指定索引的字节
  * @param  pxBuffer: 缓冲区结构体指针
  * @param  index:    要读取的字节索引
  * @retval 读取的字节，索引无效时返回'\0'
  */
char cNetBufferReadByte(NetFifoBuffer_t *pxBuffer, int index)
{
    if (pxBuffer == NULL) return '\0';
    if (index < 0 || index >= pxBuffer->ucBufferIndex)
    {
        return '\0'; // 索引无效
    }
    return pxBuffer->cNetFifoBuffer[index];
}

/**
  * @brief  比较缓冲区内容与指定字符串
  * @param  pxBuffer: 缓冲区结构体指针
  * @param  pcStr:    要比较的字符串
  * @retval true: 匹配成功, false: 匹配失败
  */
bool bNetBufferReadString(NetFifoBuffer_t *pxBuffer, const char *pcStr)
{
    if (pxBuffer == NULL || pcStr == NULL) return false;
    return (strstr(pxBuffer->cNetFifoBuffer, pcStr) != NULL);
}

/**
  * @brief  获取缓冲区当前长度
  * @param  pxBuffer: 缓冲区结构体指针
  * @retval 缓冲区长度
  */
int iNetBufferGetLength(NetFifoBuffer_t *pxBuffer)
{
    if (pxBuffer == NULL) return 0;
    return pxBuffer->ucBufferIndex;
}

/**
  * @brief  清空缓冲区内容
  * @param  pxBuffer: 缓冲区结构体指针
  * @retval None
  */
void vNetBufferClear(NetFifoBuffer_t *pxBuffer)
{
    if (pxBuffer == NULL) return;
    memset(pxBuffer->cNetFifoBuffer, 0, sizeof(pxBuffer->cNetFifoBuffer));
    pxBuffer->ucBufferIndex = 0;
}
