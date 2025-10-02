#include "IR.h"


/*****************辰哥单片机设计******************
											STM32
 * 文件			:	火焰传感器c文件                   
 * 版本			: V1.0
 * 日期			: 2024.8.10
 * MCU			:	STM32F103C8T6
 * 接口			:	见代码							
 * BILIBILI	:	辰哥单片机设计
 * CSDN			:	辰哥单片机设计
 * 作者			:	辰哥

**********************BEGIN***********************/

void IR_Init(void)
{
	#if IR_MODE
	{
		GPIO_InitTypeDef GPIO_InitStructure;
		
		RCC_APB2PeriphClockCmd (IR_AO_GPIO_CLK, ENABLE );	// 打开 ADC IO端口时钟
		GPIO_InitStructure.GPIO_Pin = IR_AO_GPIO_PIN;					// 配置 ADC IO 引脚模式
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		// 设置为模拟输入
		
		GPIO_Init(IR_AO_GPIO_PORT, &GPIO_InitStructure);				// 初始化 ADC IO

		ADCx_Init();
	}
	#else
	{
		GPIO_InitTypeDef GPIO_InitStructure;
		
		RCC_APB2PeriphClockCmd (IR_DO_GPIO_CLK, ENABLE );	// 打开连接 传感器DO 的单片机引脚端口时钟
		GPIO_InitStructure.GPIO_Pin = IR_DO_GPIO_PIN;			// 配置连接 传感器DO 的单片机引脚模式
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;			// 设置为上拉输入
		
		GPIO_Init(IR_DO_GPIO_PORT, &GPIO_InitStructure);				// 初始化 
		
	}
	#endif
	
}

#if IR_MODE
uint16_t IR_ADC_Read(void)
{
	//设置指定ADC的规则组通道，采样时间
	return ADC_GetValue(ADC_CHANNEL, ADC_SampleTime_55Cycles5);
}
#endif

uint16_t IR_FireData(void)
{
	
	#if IR_MODE
	uint32_t  tempData = 0;
	for (uint8_t i = 0; i < IR_READ_TIMES; i++)
	{
		tempData += IR_ADC_Read();
		delay_ms(5);
	}

	tempData /= IR_READ_TIMES;
	return 4095 - (uint16_t)tempData;
	
	#else
	uint16_t tempData;
	tempData = !GPIO_ReadInputDataBit(IR_DO_GPIO_PORT, IR_DO_GPIO_PIN);
	return tempData;
	#endif
}



