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
  * @brief  LoRa在传输模式下的初始化函数         
  * @note   使用USART3串口（PB10->LoRa_RX，PB11->LoRa_TX），波特率设置为9600。
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
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;																											//上拉输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = LORA_GPIO_PIN_RX;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	USART_InitStructure.USART_BaudRate = Md_Trans_BaudRate;																						//9600波特率，写完后，USART_Init函数内部会自动算好9600对应的分频系数，并写到BRR寄存器
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;										//硬件流控制,不使用流控
	USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;																			//串口模式，如果既使用输入和输出模式就用或符号，发送模式和接收模式
	USART_InitStructure.USART_Parity = USART_Parity_No;																								//无校验位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;																						//一位停止位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;																				//字长，不需要校验，字长就选择8位
	USART_Init(USART3,&USART_InitStructure);
																																																		//上面是串口的查询模式，如果使用中断，还需要开启中断，配置NVIC
	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);																											//选择RXNE的中断,并开启RXNE标志位到NVIC的输出，如果RXNE标志位置1，就会向NVIC申请中断，之后可以在中断函数中接收数据
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);																										//分组
	NVIC_InitTypeDef NVIC_InitStructure;																															//初始化NVIC的USART1通道
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;																									//中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);																																		//指向NVIC_InitStructure的地址
	USART_Cmd(USART3,ENABLE);
}

/**
  * @brief  LoRa在设置模式下的初始化函数         
  * @note   使用USART3串口（PB10->LoRa_RX，PB11->LoRa_TX），波特率设置为115200。
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
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;																											//上拉输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = LORA_GPIO_PIN_RX;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = LORA_GPIO_PIN_ATK_AUX;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;																											//上拉输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = LORA_GPIO_PIN_ATK_MD0;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	
	USART_InitTypeDef USART_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	USART_InitStructure.USART_BaudRate = Md_Set_BaudRate;																							//9600波特率，写完后，USART_Init函数内部会自动算好9600对应的分频系数，并写到BRR寄存器
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;										//硬件流控制,不使用流控
	USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;																			//串口模式，如果既使用输入和输出模式就用或符号，发送模式和接收模式
	USART_InitStructure.USART_Parity = USART_Parity_No;																								//无校验位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;																						//一位停止位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;																				//字长，不需要校验，字长就选择8位
	USART_Init(USART3,&USART_InitStructure);
																																																		//上面是串口的查询模式，如果使用中断，还需要开启中断，配置NVIC
	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);																											//选择RXNE的中断,并开启RXNE标志位到NVIC的输出，如果RXNE标志位置1，就会向NVIC申请中断，之后可以在中断函数中接收数据
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);																										//分组
	NVIC_InitTypeDef NVIC_InitStructure;																															//初始化NVIC的USART1通道
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;																									//中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);																																		//指向NVIC_InitStructure的地址
	USART_Cmd(USART3,ENABLE);
}

/**
  * @brief  LoRa发送单个字节函数
  * @note   调用这个函数，就可以从TX引脚发送一个字节数据。
  * @param  Byte
  * @retval None
  */
void LoRa_USART3_SendByte(uint8_t Byte)
{
	USART_SendData(USART3,Byte);																																			//调用这个函数，Byte就写入TDR寄存器了
																																																		//写完之后还需要等待一下，等TDR的数据转移到移位寄存器就可以了，如果数据还在TDR寄存器中，再写入数据就会产生数据覆盖，所以在发送之后还需要等待一下标志位
	while(USART_GetFlagStatus(USART3,USART_FLAG_TXE) == RESET);																				//发送数据寄存器空标志位，等待TXE置1，所以需要套一个while循环 TXE:发送数据寄存器空，发送完标志位自动置0，不用手动复位
}

/**
  * @brief  LoRa发送数组函数
  * @note   一般用于16进制模式下。
	*					uint8_t的指针类型，指向待发送数组的首地址,由于数组无法判断是否结束，所以需要再传递一个Length进来。
  * @param  *Array，Length
  * @retval None
  */
void LoRa_USART3_SendArray(uint8_t *Array,uint16_t Length)																					
{
	for(uint16_t i = 0;i < Length;i ++)																																//for循环执行Length次，可以对Array数据进行遍历，实际定义数组不要超出uint16_t的范围即可
	{
		LoRa_USART3_SendByte(Array[i]);																																	//依次取出数组Array的每一项
	}
}

/*发送字符串函数*/
void LoRa_USART3_SendString(char *String)																														//给uint8_t *也可以，由于字符串自带一个标志位，所以就不需要再传递长度参数了
{
	for(uint8_t i = 0;String[i] != '\0';i++)																													//循环结束就可以用标志位来判断了,填'\0'是空字符的转义字符表示形式，和直接写0是一样的
	{
		LoRa_USART3_SendByte(String[i]);																																//将String字符串一个个取出来，通过SendByte发送
	}
}

/**
  * @brief  printf重定向函数					
  * @note   fputc是printf函数的底层，printf函数在打印的时候，就是不断调用fputc函数打印。
  * @param  ch,*f
  * @retval ch
  */
int fputc(int ch,FILE *f)																																						
{
	LoRa_USART3_SendByte(ch);
	return ch;
}

/**
  * @brief  printf封装函数					
  * @note   第一个参数是接收字符串，第二个是接收可变参数列表。
  * @param  *format,...
  * @retval None
  */
void LoRa_USART3_Printf(char *format, ...)																													
{
	char String[100];
	va_list arg;																																											//arg是定义一个参数列表变量
	va_start(arg,format);																																							//从format位置开始接收参数表，放在arg里面
	vsprintf(String,format,arg);																																			//封装格式要用vsprintf，因为sprintf只能接收直接写的参数；打印字符串格式是format，参数表是arg，
	va_end(arg);																																											//释放参数表
	LoRa_USART3_SendString(String);
}

/**
  * @brief  LoRa发送发送hex数据包函数,也是LoRa在定点模式下执行节点间通信的主要函数。				
  * @note   hex数据包的格式为固定包长
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
