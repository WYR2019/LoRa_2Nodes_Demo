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
  * @brief  LoRa在传输模式下的初始化函数         
  * @note   使用USART3串口（PB10->LoRa_RX，PB11->LoRa_TX），波特率设置为9600。
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
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;																											//上拉输入
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Pin = LORA_GPIO_PIN_RX;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
  USART_InitTypeDef USART_InitStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
  USART_InitStructure.USART_BaudRate = mdTransBaudrate;																							//9600波特率，写完后，USART_Init函数内部会自动算好9600对应的分频系数，并写到BRR寄存器
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
void LoRa_USART3_Set_Mode_Init(uint32_t mdSetBaudrate)
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
	USART_InitStructure.USART_BaudRate = mdSetBaudrate;																								//9600波特率，写完后，USART_Init函数内部会自动算好9600对应的分频系数，并写到BRR寄存器
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
void LoRa_USART3_SendByte(uint8_t byte)
{
	USART_SendData(USART3,byte);																																			//调用这个函数，Byte就写入TDR寄存器了
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
void LoRa_USART3_SendArray(uint8_t *array,uint16_t length)																					
{
	for(uint16_t i = 0;i < length;i ++)																																//for循环执行Length次，可以对Array数据进行遍历，实际定义数组不要超出uint16_t的范围即可
	{
		LoRa_USART3_SendByte(array[i]);																																	//依次取出数组Array的每一项
	}
}

/*发送字符串函数*/
void LoRa_USART3_SendString(char *string)																														//给uint8_t *也可以，由于字符串自带一个标志位，所以就不需要再传递长度参数了
{
	for(uint8_t i = 0;string[i] != '\0';i++)																													//循环结束就可以用标志位来判断了,填'\0'是空字符的转义字符表示形式，和直接写0是一样的
	{
		LoRa_USART3_SendByte(string[i]);																																//将String字符串一个个取出来，通过SendByte发送
	}
}

/**
  * @brief  LoRa发送发送hex数据包函数,也是LoRa在定点模式下执行节点间通信的主要函数。				
  * @note   hex数据包的格式为固定包长
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
  * @brief  USART3接收中断函数、状态机 
  * @note   处理USART3接收到的数据，执行对应操作。
  * @note   状态变量一共分为3个，分别是0、1、2，也就是等待包头、接收数据和等待包尾。
  * @note   else if不会造成多个状态都被满足的问题。
  * @param  None
  * @retval None
  */
void USART3_IRQHandler(void)
{	
	/*状态变量一共分为3个，分别是0、1、2，也就是等待包头、接收执行器数据、接收执行器状态数据和等待包尾*/
	static uint8_t rxState = 0;																																				//状态变量S=0
	static uint8_t pRxPacket = 0;																																			//指示接收到哪一个数据
	if(USART_GetITStatus(USART3,USART_IT_RXNE) == SET)																								//如果RXNE置1，说明收到数据，开始根据数据处理状态。
	{
		/*接收字节，先读取到模块的变量里*/
		uint32_t rxData = USART_ReceiveData(USART3);																										//获取USART3接收到的数据
		switch(rxState)
		{
			case 0:																																												//状态1：等待包头，包头为节点号			
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
				if(pRxPacket <= 8)																																						//确保数组不越界
				{
					loRaUSART3RxPacket[pRxPacket++] = rxData;																										//每进一次接收状态，数据就转存一次缓存数组，同时存的位置++
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
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);																									//if是否要清除标志位呢，如果读取了DR，就会自动清除，如果没读取就需要手动清除
	}
}
