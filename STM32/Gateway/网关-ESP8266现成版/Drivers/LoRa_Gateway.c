#include "LoRa_Gateway.h"

uint8_t loRaNode1Addr[2]																									=	{LORA_NODE1_ADDR_HIGH,LORA_NODE1_ADDR_LOW};
uint8_t loRaNode1Channel[1]																								=	{LORA_NODE1_CHANNEL};
uint8_t loRaNode1Id[1]																										=	{LORA_NODE1_IDENTIFIER};

uint8_t loRaNode2Addr[2]																									=	{LORA_NODE2_ADDR_HIGH,LORA_NODE2_ADDR_LOW};
uint8_t loRaNode2Channel[1]																								=	{LORA_NODE2_CHANNEL};
uint8_t loRaNode2Id[1]																										=	{LORA_NODE2_IDENTIFIER};

uint8_t loRaSensorDHT11Id[1]		 																					= {0xEA};
uint8_t loRaSensorMQ2Id[1]																								=	{0xEB};
uint8_t loRaSensorLightId[1]																							=	{0xEC};
uint8_t loRaSensorFireId[1]																								= {0xED};
uint8_t	loRaExecutorFan[1]																								=	{0xFA};
uint8_t	loRaExecutorHumidifier[1]																					= {0xFB};
uint8_t	loRaExecutorBuzzer[1]																							=	{0xFC};
uint8_t	loRaExecutorLED[1]																								=	{0xFD};
uint8_t	loRaExecutorServo[1]																							=	{0xFE};
uint8_t	loRaExecutorStepmotor[1]																					=	{0xFF};

uint8_t loRaExecutorStatusOn[1]																						=	{0x01};
uint8_t loRaExecutorStatusOff[1]																					=	{0x00};

uint8_t loRaUSART3RxPacket[80];
uint8_t loRaUSART3RxData;
uint8_t loRaUSART3RxFlag = 0;

uint8_t loRaNodeID, sensorID;
int jsonTemp = 0;
int jsonHumi = 0;
int jsonFan = 0;
int jsonHumidifier = 0;
int jsonSmoke = 0;
int jsonFire = 0;
int jsonBeep = 0;
int jsonServo = 0;

/**
  * @brief  LoRa�ڴ���ģʽ�µĳ�ʼ������         
  * @note   ʹ��USART3���ڣ�PB10->LoRa_RX��PB11->LoRa_TX��������������Ϊ9600��
  * @param  Md_Trans_BaudRate
  * @retval None
  */
void LoRa_USART3_Trans_Mode_Init(uint32_t mdTransBaudrate)
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
  USART_InitStructure.USART_BaudRate = mdTransBaudrate;																							//9600�����ʣ�д���USART_Init�����ڲ����Զ����9600��Ӧ�ķ�Ƶϵ������д��BRR�Ĵ���
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
  * @brief  LoRa������ģʽ�µĳ�ʼ������         
  * @note   ʹ��USART3���ڣ�PB10->LoRa_RX��PB11->LoRa_TX��������������Ϊ115200��
  * @param  Md_Set_BaudRate
  * @retval None
  */
void LoRa_USART3_Set_Mode_Init(uint32_t mdSetBaudrate)
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
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = LORA_GPIO_PIN_ATK_AUX;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;																											//��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = LORA_GPIO_PIN_ATK_MD0;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	USART_InitStructure.USART_BaudRate = mdSetBaudrate;																								//9600�����ʣ�д���USART_Init�����ڲ����Զ����9600��Ӧ�ķ�Ƶϵ������д��BRR�Ĵ���
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
void LoRa_USART3_SendByte(uint8_t byte)
{
	USART_SendData(USART3,byte);																																			//�������������Byte��д��TDR�Ĵ�����
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
void LoRa_USART3_SendArray(uint8_t *array,uint16_t length)																					
{
	for(uint16_t i = 0;i < length;i ++)																																//forѭ��ִ��Length�Σ����Զ�Array���ݽ��б�����ʵ�ʶ������鲻Ҫ����uint16_t�ķ�Χ����
	{
		LoRa_USART3_SendByte(array[i]);																																	//����ȡ������Array��ÿһ��
	}
}

/*�����ַ�������*/
void LoRa_USART3_SendString(char *string)																														//��uint8_t *Ҳ���ԣ������ַ����Դ�һ����־λ�����ԾͲ���Ҫ�ٴ��ݳ��Ȳ�����
{
	for(uint8_t i = 0;string[i] != '\0';i++)																													//ѭ�������Ϳ����ñ�־λ���ж���,��'\0'�ǿ��ַ���ת���ַ���ʾ��ʽ����ֱ��д0��һ����
	{
		LoRa_USART3_SendByte(string[i]);																																//��String�ַ���һ����ȡ������ͨ��SendByte����
	}
}

/**
  * @brief  LoRa���ͷ���hex���ݰ�����,Ҳ��LoRa�ڶ���ģʽ��ִ�нڵ��ͨ�ŵ���Ҫ������				
  * @note   hex���ݰ��ĸ�ʽΪ�̶�����
  * @param  None
  * @retval None
  */
void LoRa_USART3_Node1IDPkt(void)
{
	LoRa_USART3_SendArray(loRaNode1Addr, 2);																													
	LoRa_USART3_SendArray(loRaNode1Channel,1);
	LoRa_USART3_SendArray(loRaNode1Id,1);
}

void LoRa_USART3_Node2IDPkt(void)
{
	LoRa_USART3_SendArray(loRaNode2Addr, 2);																													
	LoRa_USART3_SendArray(loRaNode2Channel,1);
	LoRa_USART3_SendArray(loRaNode2Id,1);
}

void LoRa_USART3_Node1_Fan_On_Msg(void)
{
	LoRa_USART3_Node1IDPkt();
	LoRa_USART3_SendArray(loRaExecutorFan,1);
	LoRa_USART3_SendArray(loRaExecutorStatusOn,1);
}

void LoRa_USART3_Node1_Fan_Off_Msg(void)
{
	LoRa_USART3_Node1IDPkt();
	LoRa_USART3_SendArray(loRaExecutorFan,1);
	LoRa_USART3_SendArray(loRaExecutorStatusOff,1);
}

void LoRa_USART3_Node1_Humidifier_On_Msg(void)
{
	LoRa_USART3_Node1IDPkt();
	LoRa_USART3_SendArray(loRaExecutorHumidifier,1);
	LoRa_USART3_SendArray(loRaExecutorStatusOn,1);
}

void LoRa_USART3_Node1_Humidifier_Off_Msg(void)
{
	LoRa_USART3_Node1IDPkt();
	LoRa_USART3_SendArray(loRaExecutorHumidifier,1);
	LoRa_USART3_SendArray(loRaExecutorStatusOff,1);
}

/**
  * @brief  USART3�����жϺ�����״̬�� 
  * @note   ����USART3���յ������ݣ�ִ�ж�Ӧ������
  * @note   ״̬����һ����Ϊ3�����ֱ���0��1��2��Ҳ���ǵȴ���ͷ���������ݺ͵ȴ���β��
  * @note   else if������ɶ��״̬������������⡣
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
		switch(rxState)
		{
			case 0:																																												//״̬1���ȴ���ͷ����ͷΪ�ڵ��			
			{
				if (rxData == 0xD1)																					
				{
					rxState = 1;
					loRaNodeID = 1;
				}
				else if (rxData == 0xD2)
				{
					rxState = 1;
					loRaNodeID = 2;
				}
				break;
			}
			case 1:
			{
				if(pRxPacket <= 8)																																						//ȷ�����鲻Խ��
				{
					loRaUSART3RxPacket[pRxPacket++] = rxData;																										//ÿ��һ�ν���״̬�����ݾ�ת��һ�λ������飬ͬʱ���λ��++
				}
				
				switch(loRaNodeID)
				{
					case 1:
					{
						if(pRxPacket == 3)
						{
							jsonTemp = loRaUSART3RxPacket[1];
							jsonHumi = loRaUSART3RxPacket[2];
							rxState = 0;
							pRxPacket = 0;
						}
						break;
					}
					case 2:
					{
						if(pRxPacket == 8)
						{
							if(loRaUSART3RxPacket[1] == 0 && loRaUSART3RxPacket[3] == 0)
							{
								jsonSmoke = 0;
								jsonFire = 0;
								jsonBeep = 0;
								jsonServo = 0;
							}
							else if(loRaUSART3RxPacket[1] == 0 && loRaUSART3RxPacket[3] == 1)
							{
								jsonSmoke = 0;
								jsonFire = 1;
								jsonBeep = 1;
								jsonServo = 1;
							}
							else if(loRaUSART3RxPacket[1] == 1 && loRaUSART3RxPacket[3] == 0)
							{
								jsonSmoke = 1;
								jsonFire = 0;
								jsonBeep = 1;
								jsonServo = 1;
							}
							else if(loRaUSART3RxPacket[1] == 1 && loRaUSART3RxPacket[3] == 1)
							{
								jsonSmoke = 1;
								jsonFire = 1;
								jsonBeep = 1;
								jsonServo = 1;
							}
							rxState = 0;
							pRxPacket = 0;
						}
					}
				}
				break;
			}
		}
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);																									//if�Ƿ�Ҫ�����־λ�أ������ȡ��DR���ͻ��Զ���������û��ȡ����Ҫ�ֶ����
	}
}
