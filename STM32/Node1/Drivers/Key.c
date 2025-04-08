#include "stm32f10x.h"                  // Device header
#include "Delay.h"
/*����*/
//������������PB0��PB10��,���������������뷽ʽʹ��
void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_10;//PB0��PB10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//������ģʽ�У��������û���ô�
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}

uint8_t Key_GetNum(void)//��������������Ϳ��Է��ذ��¼��ļ��� uint8_t����nusigned char����˼
{
	uint8_t KeyNum = 0;
	/*
		��ȡGPIO�˿ڵĹ��ܣ��ҵ�GPIO�Ķ�ȡ������uint8_t��uint16_t��ͷ��
		GPIO_ReadInputDataBit()��ȡ�������ݼĴ���ĳһ���˿ڵ�����ֵ������ֵ��uint8_t������������˿ڵĸߵ͵�ƽ����ȡ��������Ҫ�õ��������
		GPIO_ReadInputData()��ȡ��������Ĵ����ģ�����ֵ��uint16_t����һ��16λ���ݣ�ÿһλ����һ���˿�ֵ
		GPIO_ReadOutputDataBit()��ȡ����Ĵ���ĳһ���˿ڵ�ֵ
		GPIO_ReadOutputData()��ȡ��������Ĵ�����
	*/
	//PB0�������
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0) == 0)//�����ȡPB1�˿�Ϊ0��Ҳ���ǵ��������º�
	{
		Delay_ms(20);//ȥ����������
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0) == 0);//��ⰴ�����ֵ����,�������û�����֣��ͳ����������ֱ������
		Delay_ms(20);
		KeyNum = 1;//��ֵ��������=1���ݳ�ȥ
	}
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10) == 0)//�����ȡPB1�˿�Ϊ0��Ҳ���ǵ��������º�
	{
		Delay_ms(20);//ȥ����������
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10) == 0);//��ⰴ�����ֵ����,�������û�����֣��ͳ����������ֱ������
		Delay_ms(20);
		KeyNum = 2;//��ֵ��������=1���ݳ�ȥ
	}
	return KeyNum;//����Ĭ�ϸ�0�����û�а������£��ͷ���0
}
