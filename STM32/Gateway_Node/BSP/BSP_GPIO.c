#include "BSP_GPIO.h"

/**
  * @brief  vEsp8266GpioConfig
  * @note   ESP8266模块连接引脚配置函数，该函数被vEsp8266Init调用。
  * @param  无
  * @retval 无
  */
void vEsp8266GpioConfig(void)
{
    /* 定义一个GPIO_InitTypeDef类型的结构体 */
    GPIO_InitTypeDef GPIO_InitStructure;
    /* 配置 CH_PD 引脚 */
    RCC_APB2PeriphClockCmd(ESP8266_CH_PD_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin = ESP8266_CH_PD_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(ESP8266_CH_PD_PORT, &GPIO_InitStructure);
    /* 配置 RST 引脚 */
    RCC_APB2PeriphClockCmd(ESP8266_RST_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin = ESP8266_RST_PIN;
    GPIO_Init(ESP8266_RST_PORT, &GPIO_InitStructure);
}

/**
  * @brief  vLoRaGpioConfig
  * @note   初始化LoRa MD0引脚，以便进行参数设置。
  * @param  无
  * @retval 无
  */
void vLoRaGpioConfig(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(LORA_MD0_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin = LORA_MD0_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LORA_MD0_PORT, &GPIO_InitStructure);
}
