#include "LoRa_Gateway.h"

//typedef struct
//{
//	uint8_t	LoRa_Node1_Addr_High[1];
//	uint8_t	LoRa_Node1_Addr_Low[1];
//	uint8_t	LoRa_Node1_Channel[1];
//	uint8_t	LoRa_Node1_Identifier[1];
//}LoRa_Node1_Identifiers;

//typedef struct
//{
//	uint8_t	LoRa_Node2_Addr_High[1];
//	uint8_t	LoRa_Node2_Addr_Low[1];
//	uint8_t	LoRa_Node2_Channel[1];
//	uint8_t	LoRa_Node2_Identifier[1];
//}LoRa_Node2_Identifiers;

//typedef struct
//{
//	uint8_t LoRa_Sensor_DHT11_Identifier[1];
//	uint8_t LoRa_Sensor_MQ2_Identifier[1];
//	uint8_t LoRa_Sensor_Light_Identifier[1];
//	uint8_t LoRa_Sensor_RS485_Identifier[1];
//}LoRa_Sensor_Identifiers;

//typedef struct
//{
//	uint8_t LoRa_Executor_Humidifier[1];
//	uint8_t LoRa_Executor_Fan[1];
//	uint8_t LoRa_Executor_Buzzer[1];
//	uint8_t LoRa_Executor_LED[1]; 
//	uint8_t LoRa_Executor_Servo[1];
//	uint8_t LoRa_Executor_Stepmotor[1];
//}LoRa_Executor_Identifiers;

//LoRa_Node1_Identifiers Node1IDs = 
//{
//	.LoRa_Node1_Addr_High 																	= {0x03},
//	.LoRa_Node1_Addr_Low 																		= {0xEA},
//	.LoRa_Node1_Channel																			= {0x17},
//	.LoRa_Node1_Identifier																	= {0xD1}
//};

//LoRa_Node2_Identifiers Node2IDs = 
//{
//	.LoRa_Node2_Addr_High																		= {0x03},
//	.LoRa_Node2_Addr_Low																		= {0xEB},
//	.LoRa_Node2_Channel																			= {0x17},
//	.LoRa_Node2_Identifier																	= {0xD2}
//};

//LoRa_Sensor_Identifiers SensorIDs = 
//{
//	.LoRa_Sensor_DHT11_Identifier 	 												= {0xEA},
//	.LoRa_Sensor_MQ2_Identifier 														=	{0xEB},
//	.LoRa_Sensor_Light_Identifier														=	{0xEC},
//	.LoRa_Sensor_RS485_Identifier														= {0xED}
//};

//LoRa_Executor_Identifiers ExecutorsIDs = 
//{
//	.LoRa_Executor_Humidifier																= {0xFA},
//	.LoRa_Executor_Fan																			=	{0xFB},
//	.LoRa_Executor_Buzzer																		=	{0xFC},
//	.LoRa_Executor_LED																			=	{0xFD},
//	.LoRa_Executor_Servo																		=	{0xFE},
//	.LoRa_Executor_Stepmotor																=	{0xFF}
//};



typedef enum
{
	LoRa_Sensor_DHT11_Identifier
	LoRa_Sensor_MQ2_Identifier
	LoRa_Sensor_Light_Identifier
	LoRa_Sensor_RS485_Identifier
}LoRa_Sensor_Identifiers;

typedef enum
{
	LoRa_Executor_Humidifier																	= 0xFA,
	LoRa_Executor_Fan																					= 0xFB,
	LoRa_Executor_Buzzer																			= 0xFC,
	LoRa_Executor_LED																					= 0xFD,
	LoRa_Executor_Servo																				= 0xFE,
	LoRa_Executor_Stepmotor																		= 0xFF
}LoRa_Executor_Identifiers;

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
  * @brief  LoRa���ͷ���hex���ݰ�����,Ҳ��LoRa�ڶ���ģʽ��ִ�нڵ��ͨ�ŵ���Ҫ������				
  * @note   hex���ݰ��ĸ�ʽΪ�̶�����
  * @param  None
  * @retval None
  */
void LoRa_USART3_Node1IDsPkt(void)
{
	LoRa_USART3_SendArray(Node1IDs.LoRa_Node1_Addr_High, 2);																													
	LoRa_USART3_SendArray(Node1IDs.LoRa_Node1_Addr_Low,1);
	LoRa_USART3_SendArray(Node1IDs.LoRa_Node1_Channel,1);
}

void LoRa_USART3_Node2IDsPkt(void)
{
	LoRa_USART3_SendArray(Node2IDs.LoRa_Node2_Addr_High, 2);																													
	LoRa_USART3_SendArray(Node2IDs.LoRa_Node2_Addr_Low,1);
	LoRa_USART3_SendArray(Node2IDs.LoRa_Node2_Channel,1);
}
