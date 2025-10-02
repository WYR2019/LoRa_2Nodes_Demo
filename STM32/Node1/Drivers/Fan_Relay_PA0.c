#include "Fan_Relay_PA0.h"

void Fan_Relay_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RELAY_CLK, ENABLE); //≈‰÷√ ±÷”
	
  GPIO_InitStructure.GPIO_Pin = RELAY_GPIO_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(RELAY_GPIO_PORT,&GPIO_InitStructure);

  Fan_Relay_Off();
}

void Fan_Relay_On(void)
{
  GPIO_SetBits(RELAY_GPIO_PORT,RELAY_GPIO_PIN);
}

void Fan_Relay_Off()
{
  GPIO_ResetBits(RELAY_GPIO_PORT,RELAY_GPIO_PIN);
}
