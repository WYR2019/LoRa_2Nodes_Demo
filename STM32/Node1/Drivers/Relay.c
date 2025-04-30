#include "Relay.h"

/*****************���絥Ƭ�����******************
											STM32
 * �ļ�			:	5V�̵���c�ļ�                   
 * �汾			: V1.0
 * ����			: 2024.9.18
 * MCU			:	STM32F103C8T6
 * �ӿ�			:	������							
 * BILIBILI	:	���絥Ƭ�����
 * CSDN			:	���絥Ƭ�����
 * ����			:	����

**********************BEGIN***********************/


void RELAY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RELAY_CLK, ENABLE ); //����ʱ��
	
	GPIO_InitStructure.GPIO_Pin = RELAY_GPIO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(RELAY_GPIO_PROT,&GPIO_InitStructure);

	RELAY_OFF;
}
