#include "CRC32.h"

/* stm32 crc外设调用程序 */
void vCrcInit(void)
{
    /* 使能时钟 */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
    /* 复位CRC */
    CRC_ResetDR();
}

/**
  * @brief  计算32位数据的CRC
  * @param  data: 32位数据数组
  * @param  ulLength: 数据长度（32位字的个数）
  * @retval CRC计算结果
  */
uint32_t vCalculateCrc32bit(uint32_t *pulData, uint32_t ulLength)
{
    uint32_t ulCrcValue = 0;
    // uint32_t ulI = 0;
    // /* 逐字计算CRC */
    // for(ulI = 0; ulI < ululLength; ulI++)
    // {
    //     CRC_CalcBlockCRC(pulData[ulI]);
    // }
    /* 获取CRC结果 */
    ulCrcValue = CRC_CalcBlockCRC(pulData, ulLength);
    return ulCrcValue;
}
