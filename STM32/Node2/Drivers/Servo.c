#include "Servo.h"

/*****************���絥Ƭ�����******************
											STM32
 * �ļ�			:	�������ģ��c�ļ�                   
 * �汾			: V1.0
 * ����			: 2024.9.27
 * MCU			:	STM32F103C8T6
 * �ӿ�			:	������							
 * BILIBILI	:	���絥Ƭ�����
 * CSDN			:	���絥Ƭ�����
 * ����			:	����

**********************BEGIN***********************/


void Servo_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	RCC_APB2PeriphClockCmd(SERVO_CLK, ENABLE);		//����GPIOA��ʱ��
	
	/*GPIO��ʼ��*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = SERVO_GPIO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SERVO_GPIO_PORT, &GPIO_InitStructure);						//��PA1��PA2���ų�ʼ��Ϊ�������
	
	TIM_InternalClockConfig(TIM2);
	
	//����ʱ����Ԫ
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;		//ʱ�ӷ�Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;		//����ģʽ
	TIM_TimeBaseInitStructure.TIM_Period = 20000-1;			//�Զ���װARR	�ֱ���
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;		//Ԥ��ƵPSC	
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);
	
	
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);		//���ṹ�帳���ֵ 
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;			//CCR
	TIM_OC3Init(TIM2,&TIM_OCInitStructure);
	
	TIM_Cmd(TIM2,ENABLE);
}	

void PWM_SetCompare3(uint16_t Compare)
{
	TIM_SetCompare3(TIM2, Compare);		//����CCR2��ֵ
}


void Servo_SetAngle(float Angle)
{
	PWM_SetCompare3(Angle / 180 * 2000 + 500);	//����ռ�ձ�
												//���Ƕ����Ա任����Ӧ�����Ҫ���ռ�ձȷ�Χ��
}


