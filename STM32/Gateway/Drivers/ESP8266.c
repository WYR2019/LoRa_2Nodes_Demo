#include "ESP8266.h"

uint8_t esp8266RxFlag;
char esp8266RxPacket[100];//只存储发送或接收的载荷数据
uint8_t esp8266ConFinishState = 0;


void ESP8266_USART2_Init(uint32_t BaudRate)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = ESP8266_GPIO_PIN_TX;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;																											//上拉输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = ESP8266_GPIO_PIN_RX;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	USART_InitStructure.USART_BaudRate = BaudRate;																										//9600波特率，写完后，USART_Init函数内部会自动算好9600对应的分频系数，并写到BRR寄存器
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;										//硬件流控制,不使用流控
	USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;																			//串口模式，如果既使用输入和输出模式就用或符号，发送模式和接收模式
	USART_InitStructure.USART_Parity = USART_Parity_No;																								//无校验位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;																						//一位停止位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;																				//字长，不需要校验，字长就选择8位
	USART_Init(USART2,&USART_InitStructure);
																																																		//上面是串口的查询模式，如果使用中断，还需要开启中断，配置NVIC
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);																											//选择RXNE的中断,并开启RXNE标志位到NVIC的输出，如果RXNE标志位置1，就会向NVIC申请中断，之后可以在中断函数中接收数据
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);																										//分组
	NVIC_InitTypeDef NVIC_InitStructure;																															//初始化NVIC的USART2通道
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;																									//中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);																																		//指向NVIC_InitStructure的地址
	USART_Cmd(USART2,ENABLE);
}

/**
  * @brief  LoRa发送单个字节函数
  * @note   调用这个函数，就可以从TX引脚发送一个字节数据。
  * @param  Byte
  * @retval None
  */
void ESP8266_USART2_SendByte(uint8_t Byte)
{
	USART_SendData(USART2,Byte);																																			//调用这个函数，Byte就写入TDR寄存器了
																																																		//写完之后还需要等待一下，等TDR的数据转移到移位寄存器就可以了，如果数据还在TDR寄存器中，再写入数据就会产生数据覆盖，所以在发送之后还需要等待一下标志位
	while(USART_GetFlagStatus(USART2,USART_FLAG_TXE) == RESET);																				//发送数据寄存器空标志位，等待TXE置1，所以需要套一个while循环 TXE:发送数据寄存器空，发送完标志位自动置0，不用手动复位
}

/*发送字符串函数*/
void ESP8266_USART2_SendString(char *String)																												//给uint8_t *也可以，由于字符串自带一个标志位，所以就不需要再传递长度参数了
{
	for(uint8_t i = 0;String[i] != '\0';i++)																													//循环结束就可以用标志位来判断了,填'\0'是空字符的转义字符表示形式，和直接写0是一样的
	{
		ESP8266_USART2_SendByte(String[i]);																															//将String字符串一个个取出来，通过SendByte发送
	}
}

/**
  * @brief  printf封装函数					
  * @note   第一个参数是接收字符串，第二个是接收可变参数列表。
  * @param  *format,...
  * @retval None
  */
void ESP8266_USART2_Printf(char *format, ...)																													
{
	char String[100];
	va_list arg;																																											//arg是定义一个参数列表变量
	va_start(arg,format);																																							//从format位置开始接收参数表，放在arg里面
	vsprintf(String,format,arg);																																			//封装格式要用vsprintf，因为sprintf只能接收直接写的参数；打印字符串格式是format，参数表是arg，
	va_end(arg);																																											//释放参数表
	ESP8266_USART2_SendString(String);
}

//两个变量的封装
uint8_t ESP8266_GetRxFlag(void)
{
	if(esp8266RxFlag == 1)
	{
		esp8266RxFlag = 0;
		esp8266ConFinishState++;
		return 1;
	}
	return 0;
}

/**
  * @brief  ESP8266连接初始化函数					
  * @note   以下函数主要用于将ESP8266模块设置为AP模式并连接到Wi-Fi热点。
  * @param  None
  * @retval None
  */
void ESP8266_USART2_Connection_Init(void)
{
	LED_PC13_Init();
	ESP8266_USART2_Printf("AT+RESTORE\r\n");
//	LED_PC13_Twinkle();
	delay_ms(3000);
	ESP8266_USART2_Printf("AT\r\n");
	ESP8266_USART2_Printf("%d\n",strlen("AT\r\n"));
//	LED_PC13_Twinkle();
	delay_ms(3000);
	ESP8266_USART2_Printf("AT+CWMODE=1\r\n");
	ESP8266_USART2_Printf("%d\n",strlen("AT+CWMODE=1\r\n"));
//	LED_PC13_Twinkle();
	delay_ms(3000);
	ESP8266_USART2_Printf("AT+CWJAP=\"%s\",\"%s\"\r\n",WIFINAME,WIFIPASSWORD);
	ESP8266_USART2_Printf("%d\n",strlen("AT+CWJAP=\"%s\",\"%s\"\r\n"));
//	LED_PC13_Twinkle();
	delay_ms(3000);
}

/**
  * @brief  ESP8266设置MQTT模式函数					
  * @note   以下函数主要用于将ESP8266模块设置MQTT模式并连接相对应的MQTT服务器。
  * @param  None
  * @retval None
  */
void ESP8266_USART2_MQTT_Mode_Init(void)
{
	ESP8266_USART2_Printf("AT+MQTTUSERCFG=0,1,\"NULL\",\"%s\",\"%s\",0,0,\"\"\r\n",MQTTUSERNAME,MQTTPASSWORD);
//	LED_PC13_Twinkle();
	delay_ms(3000);
	ESP8266_USART2_Printf("AT+MQTTCLIENTID=0,\"%s\"\r\n",MQTTCLIENTID);
//	LED_PC13_Twinkle();
	delay_ms(3000);
	ESP8266_USART2_Printf("AT+MQTTCONN=0,\"%s\",1883,1\r\n",MQTTBROKERIP);
//	LED_PC13_Twinkle();
	delay_ms(3000);
	ESP8266_USART2_Printf("AT+MQTTSUB=0,\"%s\",1\r\n",MQTTSUBSCRIBETOPIC);
//	LED_PC13_Twinkle();
	delay_ms(3000);
}

///**
//  * @brief  USART2中断处理函数
//  *	@brief	采用16进制判断发送AT指令后ESP8266的回传字符串，用于接收ESP8266是否回传“OK”指令，如果回传则执行对应的指令。
//  * @brief	状态变量一共分为3个，分别是0、1、2，也就是等待包头、接收数据和等待包尾
//  * @param  无
//  * @retval 无
//  */
//void USART2_IRQHandler(void)
//{
//	static uint8_t RxState = 0;//状态变量S=0
//	static uint8_t pRxPacket = 0;//指示接收到哪一个变量
//	
//	if(USART_GetITStatus(USART2,USART_IT_RXNE) == SET)//如果RXNE确实置1了，就进if
//	{
//		uint8_t RxData = USART_ReceiveData(USART2);//获取接收到的数据
//		switch (RxState)
//		{
//			case 0:
//			{
//				if (RxData == 'O')
//				{
//					RxState = 1;
//					pRxPacket = 0;//清零，为下一次接收做准备
//				}
//			}
//			case 1:
//			{
//				if(RxData == '\r')//先判断是不是包尾
//				{
//					RxState = 2;
//				}
//				else
//				{	
//					esp8266RxPacket[pRxPacket++] = RxData;//每进一次接收状态，数据就转存一次缓存数组，同时存的位置++
//				}
//			}
//			case 2:
//			{
//				if (RxData == '\n')//等待第二个包尾
//				{
//					RxState = 0;//回到最初的状态
//					esp8266RxPacket[pRxPacket] = '\0';//加一个字符串结束标志位\0，以确定字符串的长度
//					esp8266RxFlag = 1;//代表整个数据包已经收到了，置一个标志位
//				}
//			}
//		}
//		USART_ClearITPendingBit(USART2,USART_IT_RXNE);//if是否要清除标志位呢，如果读取了DR，就会自动清除，如果没读取就需要手动清除
//	}
//}	
