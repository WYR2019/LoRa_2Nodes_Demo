#include "Beep.h"

/*����������*/
//�Ȳ�ҪѡA15��B3��B4�����������ӷ���������3����Ĭ����ΪJTAG�ĵ��Զ˿�
void Beep_Init(void)
{
	RCC_APB2PeriphClockCmd(BEEP_GPIO_CLOCK,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = BEEP_GPIO_PIN;//PB5
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(BEEP_GPIO_PORT,&GPIO_InitStructure);
	GPIO_SetBits(BEEP_GPIO_PORT,BEEP_GPIO_PIN);
}

void Beep_ON(void)
{
	GPIO_ResetBits(BEEP_GPIO_PORT,BEEP_GPIO_PIN);
}

void Beep_OFF(void)
{
	GPIO_SetBits(BEEP_GPIO_PORT,BEEP_GPIO_PIN);
}

void Beep_Turn(void)//ʵ�ְ�һ��Ϩ���ٰ�һ�µ��������ʵ�֣����ǰ������£�LED��״̬ȡ����ʵ�ֵ�ƽ��ת��һ���������ģʽ��
{
	if(GPIO_ReadOutputDataBit(BEEP_GPIO_PORT,BEEP_GPIO_PIN) == 0)//���PA0������Ĵ���Ϊ0
	{
		GPIO_SetBits(BEEP_GPIO_PORT,BEEP_GPIO_PIN);//��PA0��1
	}else
	{
		GPIO_ResetBits(BEEP_GPIO_PORT,BEEP_GPIO_PIN);
	}
}
