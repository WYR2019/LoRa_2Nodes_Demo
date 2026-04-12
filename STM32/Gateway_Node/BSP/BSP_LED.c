#include "BSP_LED.h"

/**
  * @brief  初始化PC13引脚为LED输出模式
  * @note   STM32F103C8T6开发板上，板载LED连接在PC13引脚上，低电平点亮LED，高电平熄灭LED
  * @param  None
  * @retval None
  */
void vPc13LedInit(void)
{
    RCC_APB2PeriphClockCmd(LED_PC13_GPIO_CLOCK, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = LED_PC13_GPIO_PIN;                //PC13
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LED_PC13_GPIO_PORT, &GPIO_InitStructure);

    GPIO_SetBits(LED_PC13_GPIO_PORT, LED_PC13_GPIO_PIN);
}

/**
  * @brief  点亮PC13引脚上的LED
  * @note   将PC13引脚输出低电平，点亮LED。如果led较多，可以定义一个LED_Set函数，然后定义两个参数控制，一个参数选择操作哪个灯，一个参数选择开还是关。
  * @param  None
  * @retval None
  */
void vPc13LedOn(void)
{
    GPIO_ResetBits(LED_PC13_GPIO_PORT, LED_PC13_GPIO_PIN);
}

void vPc13LedOff(void)
{
    GPIO_SetBits(LED_PC13_GPIO_PORT, LED_PC13_GPIO_PIN);
}

/**
 * @brief  翻转PC13引脚上的LED状态
 * @note   如果PC13引脚输出寄存器为0，则将其置1；如果为1，则将其置0。实现按一下熄灭，再按一下点亮该如何实现，就是按键按下，LED的状态取反，实现电平翻转，一般用于输出模式下的IO口。
 * @param  None
 * @retval None
 */
void vPc13LedTurn(void)
{
    if(GPIO_ReadOutputDataBit(LED_PC13_GPIO_PORT, LED_PC13_GPIO_PIN) == 0)  //如果PA0的输出寄存器为0
    {
        GPIO_SetBits(LED_PC13_GPIO_PORT, LED_PC13_GPIO_PIN);                //将PA0置1
    }else
    {
        GPIO_ResetBits(LED_PC13_GPIO_PORT, LED_PC13_GPIO_PIN);
    }
}
