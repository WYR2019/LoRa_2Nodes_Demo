#include "Relay.h"

/**
  * @brief  PA0????????         
  * @note   ???PA0???????????????????????
  * @param  None
  * @retval None
  */
void vPa0RelayInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(PA0_RELAY_CLK, ENABLE);  //≈‰÷√ ±÷”
	
    GPIO_InitStructure.GPIO_Pin = PA0_RELAY_GPIO_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(PA0_RELAY_GPIO_PORT,&GPIO_InitStructure);

    vPa0RelayOff();
}

/**
  * @brief  PA0???????         
  * @note   ?PA0???????????????
  * @param  None
  * @retval None
  */
void vPa0RelayOn(void)
{
    GPIO_SetBits(PA0_RELAY_GPIO_PORT,PA0_RELAY_GPIO_PIN);
}

/**
  * @brief  PA0???????         
  * @note   ?PA0???????????????
  * @param  None
  * @retval None
  */
void vPa0RelayOff()
{
    GPIO_ResetBits(PA0_RELAY_GPIO_PORT,PA0_RELAY_GPIO_PIN);
}
