#include "LoRa.h"

/**
  ******************************************************************************
  * @file    LoRa.c
  * @author  github/WYR2019
  * @version V1.0
  * @date    2025/5/25
  * @brief   LoRa节点1驱动
  ******************************************************************************
  * @attention
  * 1、该驱动为LoRa数据采集节点1的驱动，运行于STM32F103C8T6主控下，支持3种模式，分别
  * 为透传模式、定点模式和初始化模式，通过宏定义初始化LoRa使用模式，0为初始化模式，1为
	* 定点模式，2为透传模式。目前已完成定点模式传输的代码编写。
	* 2、LoRa数据采集节点1主要使用DHT11传感器采集温湿度，用户可在移动端远程控制风扇、
  * LED灯和加湿器。
  * 3、定点模式以16进制数据包格式传输数据，仿modbus协议进行传输。
  * 示例：03 E9 17 D1 EA 17 45
  * 其中：03 E9为LoRa网关地址的16进制数字，3E9转为10进制为1001；17为LoRa网关信道的16
  * 进制数字，转10进制为23；D1为节点1的标识，EA为DHT11传感器的标识，17、45分别为温湿
  * 度的16进制数字。
  ******************************************************************************
  */

uint8_t loRaGateAddr[2] 				 																							= {LORA_GATE_ADDR_HIGH,LORA_GATE_ADDR_LOW};
uint8_t loRaGateChannel[1]			 																							= {LORA_GATE_CHANNEL};
uint8_t loRaGateIdentifier[1]		 																							= {LORA_NODE_IDENTIFIER};

uint8_t loRaSensorDHT11Identifier[1]		 																			= {0xEA};
uint8_t loRaSensorMQ2Identifier[1]																						=	{0xEB};
uint8_t loRaSensorLightIdentifier[1]																					=	{0xEC};
uint8_t loRaSensorFireIdentifier[1]																						= {0xED};
uint8_t	loRaExecutorLED[1]																										=	{0xFA};
uint8_t	loRaExecutorFan[1]																										=	{0xFB};
uint8_t	loRaExecutorHumidifier[1]																							= {0xFC};

uint8_t loRaExecutorStatusOn[1]																								=	{0x01};
uint8_t loRaExecutorStatusOff[1]																							=	{0x00};

uint8_t loRaUSART3RxPacket[3];
uint8_t loRaUSART3RxData;
uint8_t loRaUSART3ExecutorFlag = 0;
uint8_t loRaUSART3RxFlag = 0;

uint8_t executorState,executorID;

typedef enum
{
  LoRa_Sta_Trans_Mode,
}LoRa_Mode_Init_Typedef;

/**
  * @brief  LoRa在传输模式下的初始化函数         
  * @note   使用USART3串口（PB10->LoRa_RX，PB11->LoRa_TX），波特率设置为9600。
  * @param  mdTransBaudrate
  * @retval None
  */
void LoRa_USART3_Mode_Init(void)
{
  #if LORA_MODE_INIT == 0
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Pin = LORA_GPIO_PIN_TX;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;																											
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Pin = LORA_GPIO_PIN_RX;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Pin = LORA_GPIO_PIN_ATK_MD0;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;																											
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Pin = LORA_GPIO_PIN_ATK_MD0;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_SetBits(GPIOB,LORA_GPIO_PIN_ATK_MD0);
	
  USART_InitTypeDef USART_InitStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
  USART_InitStructure.USART_BaudRate = 115200;																								
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;										
  USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;																			
  USART_InitStructure.USART_Parity = USART_Parity_No;																								
  USART_InitStructure.USART_StopBits = USART_StopBits_1;																						
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;																				
  USART_Init(USART3,&USART_InitStructure);
																																																		
  USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);																											
	
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);																										
  NVIC_InitTypeDef NVIC_InitStructure;																															
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;																									
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_Init(&NVIC_InitStructure);																																		
  USART_Cmd(USART3,ENABLE);
	
  #elif LORA_MODE_INIT
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
  USART_InitStructure.USART_BaudRate = 9600;																							          //9600波特率，写完后，USART_Init函数内部会自动算好9600对应的分频系数，并写到BRR寄存器
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

  #elif LORA_MODE_INIT == 2
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
  USART_InitStructure.USART_BaudRate = 115200;																							          
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;										
  USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;																			
  USART_InitStructure.USART_Parity = USART_Parity_No;																								
  USART_InitStructure.USART_StopBits = USART_StopBits_1;																						
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;																				
  USART_Init(USART3,&USART_InitStructure);
																																																		
  USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);																											
	
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);																										
  NVIC_InitTypeDef NVIC_InitStructure;																															
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;																									
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_Init(&NVIC_InitStructure);																																		
  USART_Cmd(USART3,ENABLE);
	
  #endif
}

/**
  * @brief  LoRa发送单个字节函数
  * @note   调用这个函数，就可以从TX引脚发送一个字节数据。
  * @param  byte
  * @retval None
  */
void LoRa_USART3_SendByte(uint8_t byte)
{
  USART_SendData(USART3,byte);                                                                      //调用这个函数，Byte就写入TDR寄存器了
                                                                                                    //写完之后还需要等待一下，等TDR的数据转移到移位寄存器就可以了，如果数据还在TDR寄存器中，再写入数据就会产生数据覆盖，所以在发送之后还需要等待一下标志位
  while(USART_GetFlagStatus(USART3,USART_FLAG_TXE) == RESET);                                       //发送数据寄存器空标志位，等待TXE置1，所以需要套一个while循环 TXE:发送数据寄存器空，发送完标志位自动置0，不用手动复位
}

#if LORA_MODE_INIT
/**
  * @brief  LoRa发送数组函数
  * @note   一般用于16进制模式下。
  * @note	  uint8_t的指针类型，指向待发送数组的首地址,由于数组无法判断是否结束，所以需要再传递一个Length进来。
  * @param  *array，length
  * @retval None
  */
void LoRa_USART3_SendArray(uint8_t *array,uint16_t length)																					
{
  for(uint16_t i = 0;i < length;i ++)                                                               //for循环执行Length次，可以对Array数据进行遍历，实际定义数组不要超出uint16_t的范围即可
  {
    LoRa_USART3_SendByte(array[i]);                                                                 //依次取出数组Array的每一项
  }
}

#else
/*发送字符串函数*/
void LoRa_USART3_SendString(char *string)                                                           //给uint8_t *也可以，由于字符串自带一个标志位，所以就不需要再传递长度参数了
{
  for(uint8_t i = 0;string[i] != '\0';i++)                                                          //循环结束就可以用标志位来判断了,填'\0'是空字符的转义字符表示形式，和直接写0是一样的
  {
    LoRa_USART3_SendByte(string[i]);                                                                //将String字符串一个个取出来，通过SendByte发送
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
  va_list arg;                                                                                      //arg是定义一个参数列表变量
  va_start(arg,format);                                                                             //从format位置开始接收参数表，放在arg里面
  vsprintf(String,format,arg);                                                                      //封装格式要用vsprintf，因为sprintf只能接收直接写的参数；打印字符串格式是format，参数表是arg，
  va_end(arg);                                                                                      //释放参数表
  LoRa_USART3_SendString(String);
}
#endif

#if LORA_MODE_INIT
/**
  * @brief  LoRa发送发送hex数据包函数,也是LoRa在定点模式下执行节点间通信的主要函数。				
  * @note   hex数据包的格式为固定包长
  * @param  None
  * @retval None
  */
void LoRa_USART3_Gate_IdPkt(void)
{
  LoRa_USART3_SendArray(loRaGateAddr, 2);                                                           //发送载荷
  LoRa_USART3_SendArray(loRaGateChannel,1);
  LoRa_USART3_SendArray(loRaGateIdentifier,1);
}



void LoRa_USART3_Send_LED_On_Status(void)
{
  
}
#endif

/**
  * @brief  USART3接收中断函数
  * @note   1、处理USART3接收到的数据，执行相应操作。
  *         2、状态变量一共分为3个，分别是等待包头（0xD1）、接收数据和等待包尾（0xAB）。
  *         3、else if不会造成多个状态都被满足的问题。
  *         4、中断只会执行第一个满足条件的if语句
  *         5、exeState分如下的状态：如LED灯：10表示收到网关的LED请求，11表示打开LED，12表示关闭LED。
  * @param  None
  * @retval None
  */
void USART3_IRQHandler(void)
{
  /*状态变量一共分为3个，分别是0、1、2，也就是等待包头、接收执行器数据、接收执行器状态数据和等待包尾*/
  static uint8_t rxState = 0;                                                                       //状态变量S=0
  static uint8_t pRxPacket = 0;                                                                     //指示接收到哪一个数据
	
  if(USART_GetITStatus(USART3,USART_IT_RXNE) == SET)                                                //如果RXNE置1，说明收到数据，开始根据数据处理状态。
  {
    /*接收字节，先读取到模块的变量里*/
    uint32_t rxData = USART_ReceiveData(USART3);                                                    //获取USART3接收到的数据
		
    #if LORA_MODE_INIT == 0
		
    #elif LORA_MODE_INIT
    switch(rxState)
    {
      case 0:                                                                                       //判断是否收到0xD1，若收到则进入数据处理状态。
			{
        if(rxData == 0xD1)                                                                          //判断包头是否正确
        {
          rxState = 1;
        }
        break;
			}
      case 1:
      {
        loRaUSART3RxPacket[pRxPacket++]	= rxData;                                                   //第pRxPacket个数据赋值给rxData，将rxData存到接收数组里。每进一次接收状态，数据就转存一次接收数组，同时存的位置++,挪到下一个位置。
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
          /* DeepSeek修改部分 */
          rxState = 0;
          pRxPacket = 0;
        }
        else
        {
          rxState = 0;
          pRxPacket = 0;
        }
        break;
      }
    }
    #elif LORA_MODE_INIT == 2
    
    #endif
		
    USART_ClearITPendingBit(USART3,USART_IT_RXNE);                                                //if是否要清除标志位呢，如果读取了DR，就会自动清除，如果没读取就需要手动清除
  }
}
