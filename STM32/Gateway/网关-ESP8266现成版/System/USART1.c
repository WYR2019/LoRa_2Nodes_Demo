#include "USART1.h"

uint16_t USART_RX_STA=0;       //����״̬���
uint8_t USART1_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�

void USART1_Init(void)
{
   //GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	 //��ʱ��
 
	
	/*  ����GPIO��ģʽ��IO�� */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//TX			   //�������PA9
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;	    //�����������
	GPIO_Init(GPIOA,&GPIO_InitStructure);  /* ��ʼ����������IO */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;//RX			 //��������PA10
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;		  //ģ������
	GPIO_Init(GPIOA,&GPIO_InitStructure); /* ��ʼ��GPIO */
	

   //USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = 115200;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART1, &USART_InitStructure); //��ʼ������1
	
	USART_Cmd(USART1, ENABLE);  //ʹ�ܴ���1 
	
	USART_ClearFlag(USART1, USART_FLAG_TC);
		
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//��������ж�

	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����	
}

int fputc(int ch,FILE *p)  //����Ĭ�ϵģ���ʹ��printf����ʱ�Զ�����
{
	USART_SendData(USART1,(u8)ch);	
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
	return ch;
}

void Serial_SendByte(uint8_t Byte)
{
	USART_SendData(USART1, Byte);		//���ֽ�����д�����ݼĴ�����д���USART�Զ�����ʱ����
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	//�ȴ��������
	/*�´�д�����ݼĴ������Զ����������ɱ�־λ���ʴ�ѭ�������������־λ*/
}

void USART1_IRQHandler(void) {
  uint8_t rxData;
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) // �����ж�(���յ������ݱ�����0x0d 0x0a �س����н�β)
	{ 
		rxData = USART_ReceiveData(USART1); // ��ȡ���յ�������
		if ((USART_RX_STA & 0x8000) == 0) // ����δ���
		{ 
			if (USART_RX_STA & 0x4000) //	���յ���0x0d
			{ 
				if (rxData != 0x0A) 
				{
					USART_RX_STA = 0;
				} 
				else 
				{
					USART_RX_STA |= 0x8000; // ���������
					ESP8266_USART1_Scan_Actions(); // ���յ����ݵĴ�����
					USART_RX_STA = 0;
				}
			} 
			else 
			{ // ��û�յ�0X0D
				if (rxData == 0x0D) 
				{
					USART_RX_STA |= 0x4000;
				} 
				else 
				{
					USART1_RX_BUF[USART_RX_STA & 0X3FFF] = rxData;
					USART_RX_STA++;
					if (USART_RX_STA > (USART_REC_LEN - 1)) 
					{
						USART_RX_STA = 0; // �������ݴ���,���¿�ʼ����
					}
				}  
			}
		}
	}
}
