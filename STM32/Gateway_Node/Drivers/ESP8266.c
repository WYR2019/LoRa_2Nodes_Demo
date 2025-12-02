#include "ESP8266.h"

void vEsp8266Restore(void)
{
    vUsartPrintf(USART2, "AT+RESTORE\r\n");
}

void vEsp8266Reset(void)
{
    vUsartPrintf(USART2, "AT+RST\r\n");
}

uint8_t ucEsp8266SetMode(uint8_t ucMode)
{
    vUsartPrintf(USART2, "AT+CWMODE=%d\r\n", ucMode);
    if (ucMode == 1)
    {
        /* code */
        return ESP8266_MODE_STATION;
    } else if (ucMode == 0)
    {
        /* code */
        return ESP8266_MODE_SOFTAP;
    } else
    {
        return 0xFF; // 无效模式
    }
}

void vEsp8266JoinAp(const char *pcSsid, const char *pcPassword)
{
    vUsartPrintf(USART2, "AT+CWJAP=\"%s\",\"%s\"\r\n", pcSsid, pcPassword);
}

/**
  * @brief  处理ESP8266响应
  * @param  *pxEspBuffer ESP8266缓冲区指针
  * @retval 0: 继续等待, 1: 成功, 2: 失败
  */
uint8_t ucEsp8266ResponseHandler(NetFifoBuffer_t *pxEspBuffer)
{
    // 检查是否收到完整响应
    if (strstr(pxEspBuffer->cResponseBuffer, "ready") != NULL)
    {
        return 1; // 成功
    } 
    else if (strstr(pxEspBuffer->cResponseBuffer, "OK") != NULL)
    {
        return 1; // 成功
    } 
    else if (strstr(pxEspBuffer->cResponseBuffer, "ERROR") != NULL || 
             strstr(pxEspBuffer->cResponseBuffer, "FAIL") != NULL)
    {
        return 2; // 失败
    }
    
    return 0; // 继续等待
}
