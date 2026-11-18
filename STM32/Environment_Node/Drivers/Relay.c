#include "Relay.h"

/**
  * @brief  PA0初始化
  * @note   
  * @param  None
  * @retval None
  */
void pa0RelayInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RELAY_PA0_CLOCK, ENABLE);
    GPIO_InitStructure.GPIO_Pin = RELAY_PA0_GPIO_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(RELAY_PA0_GPIO_PORT, &GPIO_InitStructure);

    pa0RelayOff();
}

/**
  * @brief  PA0拉高电平
  * @note   
  * @param  None
  * @retval None
  */
void pa0RelayOn(void)
{
    GPIO_SetBits(RELAY_PA0_GPIO_PORT, RELAY_PA0_GPIO_PIN);
}

/**
  * @brief  PA0拉低电平
  * @note   
  * @param  None
  * @retval None
  */
void pa0RelayOff(void)
{
    GPIO_ResetBits(RELAY_PA0_GPIO_PORT, RELAY_PA0_GPIO_PIN);
}

/**
  * @brief  判断PA0电平状态
  * @note   
  * @param  None
  * @retval None
  */
bool getPa0RelayStatus(void)
{
    return (GPIO_ReadInputDataBit(RELAY_PA0_GPIO_PORT, RELAY_PA0_GPIO_PIN) == Bit_SET);
}

/**
  * @brief  PA1初始化
  * @note   
  * @param  None
  * @retval None
  */
void pa1RelayInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RELAY_PA1_CLOCK, ENABLE);
    GPIO_InitStructure.GPIO_Pin = RELAY_PA1_GPIO_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(RELAY_PA1_GPIO_PORT, &GPIO_InitStructure);

    pa1RelayOff();    
}

/**
  * @brief  PA1拉高电平
  * @note   
  * @param  None
  * @retval None
  */
void pa1RelayOn(void)
{
    GPIO_SetBits(RELAY_PA1_GPIO_PORT, RELAY_PA1_GPIO_PIN);
}

/**
  * @brief  PA1拉低电平
  * @note   
  * @param  None
  * @retval None
  */
void pa1RelayOff(void)
{
    GPIO_ResetBits(RELAY_PA1_GPIO_PORT, RELAY_PA1_GPIO_PIN);
}

/**
  * @brief  判断PA1电平状态
  * @note   
  * @param  None
  * @retval None
  */
bool getPa1RelayStatus(void)
{
    return (GPIO_ReadInputDataBit(RELAY_PA1_GPIO_PORT, RELAY_PA1_GPIO_PIN) == Bit_SET);
}
