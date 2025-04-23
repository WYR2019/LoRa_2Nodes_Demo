#include "LED_PC13.h"
void LED_PC13_Init(void)
{
	RCC_APB2PeriphClockCmd(LED_PC13_GPIO_CLOCK,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = LED_PC13_GPIO_PIN;												//PC13
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED_PC13_PORT,&GPIO_InitStructure);
	
	GPIO_SetBits(LED_PC13_PORT,LED_PC13_GPIO_PIN);
}

/* ���led�϶࣬���Զ���һ��LED_Set������Ȼ���������������ƣ�һ������ѡ������ĸ��ƣ�һ������ѡ�񿪻��ǹ� */

void LED_PC13_ON(void)
{
	GPIO_ResetBits(LED_PC13_PORT,LED_PC13_GPIO_PIN);
}

void LED_PC13_OFF(void)
{
	GPIO_SetBits(LED_PC13_PORT,LED_PC13_GPIO_PIN);
}

/* ʵ�ְ�һ��Ϩ���ٰ�һ�µ��������ʵ�֣����ǰ������£�LED��״̬ȡ����ʵ�ֵ�ƽ��ת��һ���������ģʽ�� */
void LED_PC13_Turn(void)
{
	if(GPIO_ReadOutputDataBit(LED_PC13_PORT,LED_PC13_GPIO_PIN) == 0)								//���PA0������Ĵ���Ϊ0
	{
		GPIO_SetBits(LED_PC13_PORT,LED_PC13_GPIO_PIN);												//��PA0��1
	}else
	{
		GPIO_ResetBits(LED_PC13_PORT,LED_PC13_GPIO_PIN);
	}
}
