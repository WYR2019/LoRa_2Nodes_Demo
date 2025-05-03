#include "LoRa.h"

/*LoRa������ֻ֧�ִ��ݼ��ֽڵ����ݣ��紫�ݸ���������Ҫ�õ����ݰ�*/
/*���뺺�ַ�ֹ����ķ�����
1��ȷ�ϳ������ʹ������ֱ����ʽһ�������綼��UTF-8
2����Ҫ��C/C++�������м��������Ĳ�����--no-multibyte-chars
*/

uint8_t loRaGateAddr[2] 				 																							= {LORA_DEST_ADDR_HIGH,LORA_DEST_ADDR_LOW};
uint8_t loRaGateChannel[1]			 																							= {LORA_DEST_CHANNEL};
uint8_t loRaGateIdentifier[1]		 																							= {LORA_NODE_IDENTIFIER};

uint8_t loRaSensorDHT11Identifier[1]		 																			= {0xEA};
uint8_t loRaSensorMQ2Identifier[1]																						=	{0xEB};
uint8_t loRaSensorFireIdentifier[1]																						= {0xEC};
uint8_t loRaSensorLightIdentifier[1]																					=	{0xED};
uint8_t	loRaExecutorLED[1]																										=	{0xFA};
uint8_t	loRaExecutorHumidifier[1]																							= {0xFB};
uint8_t	loRaExecutorFan[1]																										=	{0xFC};
uint8_t	loRaExecutorBuzzer[1]																									=	{0xFD};
uint8_t	loRaExecutorServo[1]																									=	{0xFE};
uint8_t	loRaExecutorStepmotor[1]																							=	{0xFF};

uint8_t loRaExecutorStatusOn[1]																								=	{0x01};
uint8_t loRaExecutorStatusOff[1]																							=	{0x00};

uint8_t loRaUSART3RxPacket[3];
uint8_t loRaUSART3RxData;
uint8_t loRaUSART3ExecutorFlag = 0;
uint8_t loRaUSART3RxFlag = 0;

uint8_t executorState,executorID;
	
/**
  * @brief  LoRa�ĳ�ʼ������         
  * @note   ʹ��USART3���ڣ�PB10->LoRa_RX��PB11->LoRa_TX��������������Ϊ9600��
  * @param  Md_Trans_BaudRate
  * @retval None
  */
void LoRa_USART3_Trans_Mode_Init(uint32_t Md_Trans_BaudRate)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = LORA_GPIO_PIN_TX;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;																											//��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = LORA_GPIO_PIN_RX;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	USART_InitStructure.USART_BaudRate = Md_Trans_BaudRate;																						//9600�����ʣ�д���USART_Init�����ڲ����Զ����9600��Ӧ�ķ�Ƶϵ������д��BRR�Ĵ���
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;										//Ӳ��������,��ʹ������
	USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;																			//����ģʽ�������ʹ����������ģʽ���û���ţ�����ģʽ�ͽ���ģʽ
	USART_InitStructure.USART_Parity = USART_Parity_No;																								//��У��λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;																						//һλֹͣλ
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;																				//�ֳ�������ҪУ�飬�ֳ���ѡ��8λ
	USART_Init(USART3,&USART_InitStructure);
																																																		//�����Ǵ��ڵĲ�ѯģʽ�����ʹ���жϣ�����Ҫ�����жϣ�����NVIC
	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);																											//ѡ��RXNE���ж�,������RXNE��־λ��NVIC����������RXNE��־λ��1���ͻ���NVIC�����жϣ�֮��������жϺ����н�������
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);																										//����
	NVIC_InitTypeDef NVIC_InitStructure;																															//��ʼ��NVIC��USART1ͨ��
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;																									//�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);																																		//ָ��NVIC_InitStructure�ĵ�ַ
	USART_Cmd(USART3,ENABLE);
}

/**
  * @brief  LoRa���͵����ֽں���
  * @note   ��������������Ϳ��Դ�TX���ŷ���һ���ֽ����ݡ�
  * @param  Byte
  * @retval None
  */
void LoRa_USART3_SendByte(uint8_t Byte)
{
	USART_SendData(USART3,Byte);																																			//�������������Byte��д��TDR�Ĵ�����
																																																		//д��֮����Ҫ�ȴ�һ�£���TDR������ת�Ƶ���λ�Ĵ����Ϳ����ˣ�������ݻ���TDR�Ĵ����У���д�����ݾͻ�������ݸ��ǣ������ڷ���֮����Ҫ�ȴ�һ�±�־λ
	while(USART_GetFlagStatus(USART3,USART_FLAG_TXE) == RESET);																				//�������ݼĴ����ձ�־λ���ȴ�TXE��1��������Ҫ��һ��whileѭ�� TXE:�������ݼĴ����գ��������־λ�Զ���0�������ֶ���λ
}

/**
  * @brief  LoRa�������麯��
  * @note   һ������16����ģʽ�¡�
	*					uint8_t��ָ�����ͣ�ָ�������������׵�ַ,���������޷��ж��Ƿ������������Ҫ�ٴ���һ��Length������
  * @param  *Array��Length
  * @retval None
  */
void LoRa_USART3_SendArray(uint8_t *Array,uint16_t Length)																					
{
	for(uint16_t i = 0;i < Length;i ++)																																//forѭ��ִ��Length�Σ����Զ�Array���ݽ��б�����ʵ�ʶ������鲻Ҫ����uint16_t�ķ�Χ����
	{
		LoRa_USART3_SendByte(Array[i]);																																	//����ȡ������Array��ÿһ��
	}
}

/**
  * @brief  LoRa����hex���ݰ�����,Ҳ��LoRa�ڶ���ģʽ��ִ�нڵ��ͨ�ŵ���Ҫ������			
  * @note   hex���ݰ��ĸ�ʽΪ�̶�����
  * @param  None
  * @retval None
  */
void LoRa_USART3_IdentifierPkt(void)
{
	LoRa_USART3_SendArray(loRaGateAddr, 2);																													//�����غ�
	LoRa_USART3_SendArray(loRaGateChannel,1);
	LoRa_USART3_SendArray(loRaGateIdentifier,1);
}

/*����hex���ݰ�*/

/*�����ַ�������*/
void LoRa_USART3_SendString(char *String)																														//��uint8_t *Ҳ���ԣ������ַ����Դ�һ����־λ�����ԾͲ���Ҫ�ٴ��ݳ��Ȳ�����
{
	for(uint8_t i = 0;String[i] != '\0';i++)																													//ѭ�������Ϳ����ñ�־λ���ж���,��'\0'�ǿ��ַ���ת���ַ���ʾ��ʽ����ֱ��д0��һ����
	{
		LoRa_USART3_SendByte(String[i]);																																//��String�ַ���һ����ȡ������ͨ��SendByte����
	}
}

/**
  * @brief  printf�ض�����					
  * @note   fputc��printf�����ĵײ㣬printf�����ڴ�ӡ��ʱ�򣬾��ǲ��ϵ���fputc������ӡ��
  * @param  ch,*f
  * @retval ch
  */
int fputc(int ch,FILE *f)																																						
{
	LoRa_USART3_SendByte(ch);
	return ch;
}

/**
  * @brief  printf��װ����					
  * @note   ��һ�������ǽ����ַ������ڶ����ǽ��տɱ�����б�
  * @param  *format,...
  * @retval None
  */
void LoRa_USART3_Printf(char *format, ...)																													
{
	char String[100];
	va_list arg;																																											//arg�Ƕ���һ�������б����
	va_start(arg,format);																																							//��formatλ�ÿ�ʼ���ղ���������arg����
	vsprintf(String,format,arg);																																			//��װ��ʽҪ��vsprintf����Ϊsprintfֻ�ܽ���ֱ��д�Ĳ�������ӡ�ַ�����ʽ��format����������arg��
	va_end(arg);																																											//�ͷŲ�����
	LoRa_USART3_SendString(String);
}

/**
  * @brief  USART3�����жϺ���					
  * @note   ����USART3���յ������ݣ�ִ�ж�Ӧ������
  * @param  None
  * @retval None
  */
void USART3_IRQHandler(void)
{
	/*״̬����һ����Ϊ3�����ֱ���0��1��2��Ҳ���ǵȴ���ͷ������ִ�������ݡ�����ִ����״̬���ݺ͵ȴ���β*/
	static uint8_t rxState = 0;																																				//״̬����S=0
	static uint8_t pRxPacket = 0;																																			//ָʾ���յ���һ������
	
	if(USART_GetITStatus(USART3,USART_IT_RXNE) == SET)																								//���RXNE��1��˵���յ����ݣ���ʼ�������ݴ���״̬��
	{
		/*�����ֽڣ��ȶ�ȡ��ģ��ı�����*/
		uint32_t rxData = USART_ReceiveData(USART3);																										//��ȡUSART3���յ�������
		if(rxState == 0)																																								//�ж��Ƿ��յ�0xD1�����յ���������ݴ���״̬��
		{
			if(rxData == 0xD1)																																						//�жϰ�ͷ�Ƿ���ȷ
			{
				rxState = 1;
			}
		}
		else if(rxState == 1)
		{
			loRaUSART3RxPacket[pRxPacket++]	= rxData;																											//��pRxPacket�����ݸ�ֵ��rxData����rxData�浽���������ÿ��һ�ν���״̬�����ݾ�ת��һ�ν������飬ͬʱ���λ��++,Ų����һ��λ�á�
			if(pRxPacket == 1)
			{
				if(loRaUSART3RxPacket[0] == 0xFA)	
					executorID = 10;
				else if(loRaUSART3RxPacket[0] == 0xFB) 
					executorID = 20;
				else if(loRaUSART3RxPacket[0] == 0xFC) 
					executorID = 30;
			}
			else if(pRxPacket == 2)
			{
				if(executorID == 10)
				{
					if(loRaUSART3RxPacket[1] == 0x01) 
						executorState = 11;
					else if(loRaUSART3RxPacket[1] == 0x00) 
						executorState = 12;
				}
				else if(executorID == 20)
				{
					if(loRaUSART3RxPacket[1] == 0x01) 
						executorState = 21;
					else if(loRaUSART3RxPacket[1] == 0x00) 
						executorState = 22;
				}
				else if(executorID == 30)
				{
					if(loRaUSART3RxPacket[1] == 0x01) 
						executorState = 31;
					else if(loRaUSART3RxPacket[1] == 0x00) 
						executorState = 32;
				}
				/* DeepSeek�޸Ĳ��� */
				rxState = 0;
				pRxPacket = 0;
			}
			else
			{
				rxState = 0;
				pRxPacket = 0;
			}
		}
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);																									//if�Ƿ�Ҫ�����־λ�أ������ȡ��DR���ͻ��Զ���������û��ȡ����Ҫ�ֶ����
	}
}

