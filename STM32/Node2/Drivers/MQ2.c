#include "MQ2.h"

/*****************���絥Ƭ�����******************
											STM32
 * �ļ�			:	MQ-2��������c�ļ�                   
 * �汾			: V1.0
 * ����			: 2024.8.21
 * MCU			:	STM32F103C8T6
 * �ӿ�			:	������							
 * BILIBILI	:	���絥Ƭ�����
 * CSDN			:	���絥Ƭ�����
 * ����			:	����

**********************BEGIN***********************/

void MQ2_Init(void)
{
	#if MODE
	{
		GPIO_InitTypeDef GPIO_InitStructure;
		
		RCC_APB2PeriphClockCmd (MQ2_AO_GPIO_CLK, ENABLE );	// �� ADC IO�˿�ʱ��
		GPIO_InitStructure.GPIO_Pin = MQ2_AO_GPIO_PIN;					// ���� ADC IO ����ģʽ
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		// ����Ϊģ������
		
		GPIO_Init(MQ2_AO_GPIO_PORT, &GPIO_InitStructure);				// ��ʼ�� ADC IO

		ADCx_Init();
	}
	#else
	{
		GPIO_InitTypeDef GPIO_InitStructure;
		
		RCC_APB2PeriphClockCmd (MQ2_DO_GPIO_CLK, ENABLE );	// ������ ������DO �ĵ�Ƭ�����Ŷ˿�ʱ��
		GPIO_InitStructure.GPIO_Pin = MQ2_DO_GPIO_PIN;			// �������� ������DO �ĵ�Ƭ������ģʽ
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;			// ����Ϊ��������
		
		GPIO_Init(MQ2_DO_GPIO_PORT, &GPIO_InitStructure);				// ��ʼ�� 
		
	}
	#endif
	
}

#if MODE
uint16_t MQ2_ADC_Read(void)
{
	//����ָ��ADC�Ĺ�����ͨ��������ʱ��
	return ADC_GetValue(ADC_CHANNEL, ADC_SampleTime_55Cycles5);
}
#endif

uint16_t MQ2_GetData(void)
{
	
	#if MODE
	uint32_t  tempData = 0;
	for (uint8_t i = 0; i < MQ2_READ_TIMES; i++)
	{
		tempData += MQ2_ADC_Read();
		delay_ms(5);
	}

	tempData /= MQ2_READ_TIMES;
	return tempData;
	
	#else
	uint16_t tempData;
	tempData = !GPIO_ReadInputDataBit(MQ2_DO_GPIO_PORT, MQ2_DO_GPIO_PIN);
	return tempData;
	#endif
}


float MQ2_GetData_PPM(void)
{
	#if MODE
	float  tempData = 0;
	

	for (uint8_t i = 0; i < MQ2_READ_TIMES; i++)
	{
		tempData += MQ2_ADC_Read();
		delay_ms(5);
	}
	tempData /= MQ2_READ_TIMES;
	
	float Vol = (tempData*5/4096);
	float RS = (5-Vol)/(Vol*0.5);
	float R0=6.64;
	
	float ppm = pow(11.5428*R0/RS, 0.6549f);
	
	return ppm;
	#endif
}
