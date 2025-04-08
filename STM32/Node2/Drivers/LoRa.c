#include "LoRa.h"

/*LoRa驱动，只支持传递简单字节的数据，如传递复杂数据需要用到数据包*/
/*输入汉字防止乱码的方法：
1、确认程序编码和串口助手编码格式一样，比如都是UTF-8
2、需要在C/C++、杂项中加上这样的参数：--no-multibyte-chars
*/

uint8_t LoRa_USART3_RxPacket[4];
uint8_t LoRa_USART3_RXData;
uint8_t LoRa_USART3_RXFlag;

uint8_t LoRa_Dest_Addr[2] 				 														= {LORA_DEST_ADDR_HIGH,LORA_DEST_ADDR_LOW};
uint8_t LoRa_Dest_Channel[1]			 														= {LORA_DEST_CHANNEL};
uint8_t LoRa_Node_Identifier[1]		 														= {LORA_NODE_IDENTIFIER};
uint8_t LoRa_Sensor_Identifier[1]		 													= {0xEC};
	
/**
  * @brief  LoRa的初始化函数         
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

/**
  * @brief  LoRa发送hex数据包函数,也是LoRa在定点模式下执行节点间通信的主要函数。			
  * @note   hex数据包的格式为固定包长
  * @param  None
  * @retval None
  */
void LoRa_USART3_IdentifierPkt(void)
{
	LoRa_USART3_SendArray(LoRa_Dest_Addr, 2);																													//发送载荷
	LoRa_USART3_SendArray(LoRa_Dest_Channel,1);
	LoRa_USART3_SendArray(LoRa_Node_Identifier,1);
	LoRa_USART3_SendArray(LoRa_Sensor_Identifier,1);
}

/*接收hex数据包*/

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

/*两个变量的封装*/
uint8_t LoRa_USART3_GetRxFlag(void)
{
	if(LoRa_USART3_RXFlag == 1)
	{
		LoRa_USART3_RXFlag = 0;
		return 1;
	}
	return 0;
}

/**
  * @brief  USART3接收中断函数					
  * @note   处理USART3接收到的数据，执行对应操作。
  * @param  None
  * @retval None
  */
//void USART3_IRQHandler(void)
//{
//	//用状态机接收逻辑
//	//状态变量一共分为3个，分别是0、1、2，也就是等待包头、接收数据和等待包尾
//	static uint8_t RxState = 0;																																				//状态变量S=0
//	static uint8_t pRxPacket = 0;																																			//指示接收到哪一个变量
//	if(USART_GetITStatus(USART3,USART_IT_RXNE) == SET)																								//如果RXNE确实置1了，就进if
//	{
//		//接收字节，先读取到模块的变量里
//		//注意：如果使用3个if，可能会导致其中两个或3个条件同时成立的问题
//		
//		uint8_t RxData = USART_ReceiveData(USART3);																											//获取RxData
//		if (RxState == 0)
//		{
//			if (RxData == 0xFF)
//			{
//				RxState = 1;
//				pRxPacket = 0;																																							//清零，为下一次接收做准备
//			}
//		}
//		else if (RxState == 1)
//		{
//			LoRa_USART3_RxPacket[pRxPacket] = RxData;                                                     //每进一次接收状态，数据就转存一次缓存数组，同时存的位置++
//			pRxPacket++;																																									//移动到下一个位置
//			if(pRxPacket >= 4)
//			{
//				RxState = 2;
//			}
//		}
//		else if (RxState == 2)
//		{
//			if (RxData == 0xFE)
//			{
//				RxState = 0;																																								//回到最初的状态
//				LoRa_USART3_RXFlag = 1;																																			//代表整个数据包已经收到了，置一个标志位
//			}
//		}
//		USART_ClearITPendingBit(USART3,USART_IT_RXNE);																									//if是否要清除标志位呢，如果读取了DR，就会自动清除，如果没读取就需要手动清除
//	}
//}
