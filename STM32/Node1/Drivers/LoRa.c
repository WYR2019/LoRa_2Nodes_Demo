#include "LoRa.h"
#include "LED_PC13.h"

/*LoRa������ֻ֧�ִ��ݼ��ֽڵ����ݣ��紫�ݸ���������Ҫ�õ����ݰ�*/
/*���뺺�ַ�ֹ����ķ�����
1��ȷ�ϳ������ʹ������ֱ����ʽһ�������綼��UTF-8
2����Ҫ��C/C++�������м��������Ĳ�����--no-multibyte-chars
*/

uint8_t LoRa_Gate_Addr[2] 				 																							= {LORA_GATE_ADDR_HIGH,LORA_GATE_ADDR_LOW};
uint8_t LoRa_Gate_Channel[1]			 																							= {LORA_GATE_CHANNEL};
uint8_t LoRa_Node_Identifier[1]		 																							= {LORA_NODE_IDENTIFIER};

uint8_t LoRa_Sensor_DHT11_Identifier[1]		 																			= {0xEA};
uint8_t LoRa_Sensor_MQ2_Identifier[1]																						=	{0xEB};
uint8_t LoRa_Sensor_Light_Identifier[1]																					=	{0xEC};
uint8_t LoRa_Sensor_RS485_Identifier[1]																					= {0xED};
uint8_t	LoRa_Executor_Humidifier[1]																							= {0xFA};
uint8_t	LoRa_Executor_Fan[1]																										=	{0xFB};
uint8_t	LoRa_Executor_Buzzer[1]																									=	{0xFC};
uint8_t	LoRa_Executor_LED[1]																										=	{0xFD};
uint8_t	LoRa_Executor_Servo[1]																									=	{0xFE};
uint8_t	LoRa_Executor_Stepmotor[1]																							=	{0xFF};

uint8_t LoRa_LED_Status_On[1]																										=	{0x01};
uint8_t LoRa_LED_Status_Off[1]																									=	{0x00};

uint8_t LoRa_USART3_RxPacket[3];
uint8_t LoRa_USART3_RXData;
uint8_t LoRa_USART3_Executor_Flag = 0;
uint8_t LoRa_USART3_RXFlag;

/**
  * @brief  LoRa�ڴ���ģʽ�µĳ�ʼ������         
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
  * @brief  LoRa������ģʽ�µĳ�ʼ������         
  * @note   ʹ��USART3���ڣ�PB10->LoRa_RX��PB11->LoRa_TX��������������Ϊ115200��
  * @param  Md_Set_BaudRate
  * @retval None
  */
void LoRa_USART3_Set_Mode_Init(uint32_t Md_Set_BaudRate)
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
	USART_InitStructure.USART_BaudRate = Md_Set_BaudRate;																							//9600�����ʣ�д���USART_Init�����ڲ����Զ����9600��Ӧ�ķ�Ƶϵ������д��BRR�Ĵ���
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
  * @brief  ��ȡ��־λ					
  * @note   ����LoRa�յ������ݰ�����
  * @param  None
  * @retval 1��2��99
  */
uint8_t LoRa_USART3_GetRxFlag(void)
{
//	if(LoRa_USART3_RXFlag == 1)
//	{
//		LoRa_USART3_RXFlag = 0;
//		return 1;
//	}
	if(LoRa_USART3_RXFlag == 2)
	{
		LoRa_USART3_RXFlag = 0;
		return 1;
	}
	else if(LoRa_USART3_RXFlag == 3)
	{
		LoRa_USART3_RXFlag = 0;
		return 2;
	}
	return 0;
}

/**
  * @brief  LoRa���ͷ���hex���ݰ�����,Ҳ��LoRa�ڶ���ģʽ��ִ�нڵ��ͨ�ŵ���Ҫ������				
  * @note   hex���ݰ��ĸ�ʽΪ�̶�����
  * @param  None
  * @retval None
  */
void LoRa_USART3_IdentifierPkt(void)
{
	LoRa_USART3_SendArray(LoRa_Gate_Addr, 2);																													//�����غ�
	LoRa_USART3_SendArray(LoRa_Gate_Channel,1);
	LoRa_USART3_SendArray(LoRa_Node_Identifier,1);
}

/**
  * @brief  USART3�����жϺ�����״̬�� 
  * @note   ����USART3���յ������ݣ�ִ�ж�Ӧ������
	* @note		״̬����һ����Ϊ3�����ֱ���0��1��2��Ҳ���ǵȴ���ͷ���������ݺ͵ȴ���β��
	* @note		else if������ɶ��״̬������������⡣
  * @param  None
  * @retval None
  */
void USART3_IRQHandler(void)
{
	/*״̬����һ����Ϊ3�����ֱ���0��1��2��Ҳ���ǵȴ���ͷ������ִ�������ݡ�����ִ����״̬���ݺ͵ȴ���β*/
	static uint8_t RxState = 0;																																				//״̬����S=0
	static uint8_t pRxPacket = 0;																																			//ָʾ���յ���һ������
	if(USART_GetITStatus(USART3,USART_IT_RXNE) == SET)																								//���RXNE��1��˵���յ����ݣ���ʼ�������ݴ���״̬��
	{
		/*�����ֽڣ��ȶ�ȡ��ģ��ı�����*/
		uint32_t RxData = USART_ReceiveData(USART3);																										//��ȡUSART3���յ�������
		{
			if (RxData == 0x0C)																																						//״̬1���ȴ���ͷ����ͷΪ���һλ�ŵ���
			{
				RxState = 1;
				pRxPacket = 0;																																							//��ǰ���㣬Ϊ��һ�ν�����׼��
			}
			else if (RxState == 1)																																				//״̬2����������
			{
				LoRa_USART3_RxPacket[pRxPacket] = RxData;                                                   //ÿ��һ�ν���״̬�����ݾ�ת��һ�λ������飬ͬʱ���λ��++
				pRxPacket++;																																								//�ƶ�����һ��λ��
				if(pRxPacket == 1)
				{
					if(LoRa_USART3_RxPacket[0] == 0x0A)
					{
						LoRa_USART3_Executor_Flag = 3;
					}
				}
				else if(pRxPacket == 2)
				{
					if(LoRa_USART3_Executor_Flag == 3 && LoRa_USART3_RxPacket[1] == 0x01)
					{
						LoRa_USART3_RXFlag = 2;
					}
					else if(LoRa_USART3_Executor_Flag == 3 && LoRa_USART3_RxPacket[1] == 0x05)
					{
						LoRa_USART3_RXFlag = 3;
					}
				}
				else if(pRxPacket > 2)
				{
					RxState = 2;
				}
			}
			else if (RxState == 2)																																					//״̬3���ȴ���β
			{
				if (RxData == 0xAB)
				{
					RxState = 0;																																								//�ص������״̬
					LoRa_USART3_RXFlag = 1;																																			//�����������ݰ��Ѿ��յ��ˣ���һ����־λ
				}
			}
			USART_ClearITPendingBit(USART3,USART_IT_RXNE);																									//if�Ƿ�Ҫ�����־λ�أ������ȡ��DR���ͻ��Զ���������û��ȡ����Ҫ�ֶ����
		}
	}
}
