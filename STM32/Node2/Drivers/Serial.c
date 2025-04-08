#include "Serial.h"

/*USART1驱动，只支持传递简单字节的数据，如传递复杂数据需要用到数据包*/
/*输入汉字防止乱码的方法：
1、确认程序编码和串口助手编码格式一样，比如都是UTF-8
2、需要在C/C++、杂项中加上这样的参数：--no-multibyte-chars
*/

uint8_t Serial_RXData;
uint8_t Serial_RXFlag;

uint8_t Serial_TxPacket[9];//只存储发送或接收的载荷数据
uint8_t Serial_RxPacket[4];

void Serial_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;//串口发送，PA9
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//串口接收，PA10
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;//9600波特率，写完后，USART_Init函数内部会自动算好9600对应的分频系数，并写到BRR寄存器
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//硬件流控制,不使用流控
	USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;//串口模式，如果既使用输入和输出模式就用或符号，发送模式和接收模式
	USART_InitStructure.USART_Parity = USART_Parity_No;//无校验位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一位停止位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长，不需要校验，字长就选择8位
	USART_Init(USART1,&USART_InitStructure);
	//上面是串口的查询模式，如果使用中断，还需要开启中断，配置NVIC
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);//选择RXNE的中断,并开启RXNE标志位到NVIC的输出，如果RXNE标志位置1，就会向NVIC申请中断，之后可以在中断函数中接收数据
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//分组
	NVIC_InitTypeDef NVIC_InitStructure;//初始化NVIC的USART1通道
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);//指向NVIC_InitStructure的地址
	USART_Cmd(USART1,ENABLE);
	
}

/*发送单个字节函数，调用这个函数，就可以从TX引脚发送一个字节数据*/
void Serial_SendByte(uint8_t Byte)
{
	USART_SendData(USART1,Byte);//调用这个函数，Byte就写入TDR寄存器了
	//写完之后还需要等待一下，等TDR的数据转移到移位寄存器就可以了，如果数据还在TDR寄存器中，再写入数据就会产生数据覆盖，所以在发送之后还需要等待一下标志位
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);//发送数据寄存器空标志位，等待TXE置1，所以需要套一个while循环 TXE:发送数据寄存器空，发送完标志位自动置0，不用手动复位
}

/*发送数组函数，一般用于hex模式下*/
void Serial_SendArray(uint8_t *Array,uint16_t Length)//uint8_t的指针类型，指向待发送数组的首地址,由于数组无法判断是否结束，所以需要再传递一个Length进来
{
	for(uint16_t i = 0;i < Length;i ++)//for循环执行Length次，可以对Array数据进行遍历，实际定义数组不要超出uint16_t的范围即可
	{
		Serial_SendByte(Array[i]);//依次取出数组Array的每一项
	}
}

/*发送字符串函数*/
void Serial_SendString(char *String)//给uint8_t *也可以，由于字符串自带一个标志位，所以就不需要再传递长度参数了
{
	for(uint8_t i = 0;String[i] != '\0';i++)//循环结束就可以用标志位来判断了,填'\0'是空字符的转义字符表示形式，和直接写0是一样的
	{
		Serial_SendByte(String[i]);//将String字符串一个个取出来，通过SendByte发送
	}
}

/*printf函数重定向*/
int fputc(int ch,FILE *f)//fputc是printf函数的底层，printf函数在打印的时候，就是不断调用fputc函数打印
{
	Serial_SendByte(ch);
	return ch;
}

/*printf函数封装*/
void Serial_Printf(char *format, ...)//第一个是接收字符串，第二个是接收可变参数列表
{
	char String[100];
	va_list arg;//定义一个参数列表变量
	va_start(arg,format);//从format位置开始接收参数表，放在arg里面
	vsprintf(String,format,arg);//封装格式要用vsprintf，因为sprintf只能接收直接写的参数；打印字符串格式是format，参数表是arg，
	va_end(arg);//释放参数表
	Serial_SendString(String);
}

/*发送hex数据包，hex数据包的格式为固定包长，含包头包尾，其中包头为FF，载荷数据固定为4字节，包尾为FE*/
void Serial_SendPacket(void)
{
	Serial_SendArray(Serial_TxPacket, 9);//发送载荷
}

/*接收hex数据包*/

/*两个变量的封装*/
uint8_t Serial_GetRxFlag(void)
{
	if(Serial_RXFlag == 1)
	{
		Serial_RXFlag = 0;
		return 1;
	}
	return 0;
}

void USART1_IRQHandler(void)
{
	//用状态机接收逻辑
	//状态变量一共分为3个，分别是0、1、2，也就是等待包头、接收数据和等待包尾
	static uint8_t RxState = 0;//状态变量S=0
	static uint8_t pRxPacket = 0;//指示接收到哪一个变量
	
	if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET)//如果RXNE确实置1了，就进if
	{
		//接收字节，先读取到模块的变量里
		//注意：如果使用3个if，可能会导致其中两个或3个条件同时成立的问题
		
		uint8_t RxData = USART_ReceiveData(USART1);//获取RxData
		if (RxState == 0)
		{
			if (RxData == 0xFF)
			{
				RxState = 1;
				pRxPacket = 0;//清零，为下一次接收做准备
			}
		}
		else if (RxState == 1)
		{
			Serial_RxPacket[pRxPacket] = RxData;//每进一次接收状态，数据就转存一次缓存数组，同时存的位置++
			pRxPacket++;//移动到下一个位置
			if(pRxPacket >= 4)
			{
				RxState = 2;
			}
		}
		else if (RxState == 2)
		{
			if (RxData == 0xFE)
			{
				RxState = 0;//回到最初的状态
				Serial_RXFlag = 1;//代表整个数据包已经收到了，置一个标志位
			}
		}
		
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);//if是否要清除标志位呢，如果读取了DR，就会自动清除，如果没读取就需要手动清除
	}
}
