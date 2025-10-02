#include "LoRa_Gateway.h"

uint8_t loRaUSART3RxPacket[80];

uint8_t loRaNodeID, sensorID;

/**
  * @brief  LoRa的初始化函数         
  * @note   使用USART3串口（PB10->LoRa_RX，PB11->LoRa_TX），波特率设置为115200。
  * @param  loRaUSART3Baudrate
  * @retval None
  */
void vLoRaUSART3EnableInit(uint32_t ulLoRaUSART3Baudrate)
{
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

  USART_InitTypeDef USART_InitStructure;
  USART_InitStructure.USART_BaudRate = ulLoRaUSART3Baudrate;                                        //设定后，USART_Init函数内部会自动算好9600对应的分频系数，并写到BRR寄存器
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;                   //硬件流控制,不使用流控
  USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;                                     //串口模式，如果既使用输入和输出模式就用或符号，发送模式和接收模式
  USART_InitStructure.USART_Parity = USART_Parity_No;                                               //无校验位
  USART_InitStructure.USART_StopBits = USART_StopBits_1;                                            //一位停止位
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;                                       //字长，不需要校验，字长就选择8位
  USART_Init(USART3,&USART_InitStructure);
                                                                                                    //上面是串口的查询模式，如果使用中断，还需要开启中断，配置NVIC
  USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);                                                      //选择RXNE的中断,并开启RXNE标志位到NVIC的输出，如果RXNE标志位置1，就会向NVIC申请中断，之后可以在中断函数中接收数据

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);                                                   //分组
  NVIC_InitTypeDef NVIC_InitStructure;                                                              //初始化NVIC的USART1通道
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;                                                 //中断通道
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_Init(&NVIC_InitStructure);                                                                   //指向NVIC_InitStructure的地址

  USART_Cmd(USART3,ENABLE);

  #if LORA_MODE_INIT == 0

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = LORA_GPIO_PIN_ATK_MD0;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;																											
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = LORA_GPIO_PIN_ATK_AUX;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB,LORA_GPIO_PIN_ATK_MD0);

  #elif LORA_MODE_INIT == 2
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = LORA_GPIO_PIN_ATK_MD0;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

  #endif
}

/**
  * @brief  LoRa发送单个字节函数
  * @note   调用这个函数，就可以从TX引脚发送一个字节数据。
  * @param  Byte
  * @retval None
  */
void vLoRaUSART3SendByte(uint8_t ucByte)
{
  USART_SendData(USART3,ucByte);                                                                      //调用这个函数，Byte就写入TDR寄存器了
                                                                                                      //写完之后还需要等待一下，等TDR的数据转移到移位寄存器就可以了，如果数据还在TDR寄存器中，再写入数据就会产生数据覆盖，所以在发送之后还需要等待一下标志位
  while(USART_GetFlagStatus(USART3,USART_FLAG_TXE) == RESET);                                         //发送数据寄存器空标志位，等待TXE置1，所以需要套一个while循环 TXE:发送数据寄存器空，发送完标志位自动置0，不用手动复位
}

#if LORA_MODE_INIT
  /**
    * @brief  LoRa发送数组函数
    * @note   一般用于16进制模式下。
    * @note	  uint8_t的指针类型，指向待发送数组的首地址,由于数组无法判断是否结束，所以需要再传递一个Length进来。
    * @param  *array，length
    * @retval None
    */
  void vLoRaUSART3SendArray(uint8_t *pucArray,uint16_t usLength)																					
  {
    for(uint16_t i = 0;i < usLength;i ++)                                                             //for循环执行Length次，可以对Array数据进行遍历，实际定义数组不要超出uint16_t的范围即可
    {
      vLoRaUSART3SendByte(pucArray[i]);                                                               //依次取出数组Array的每一项
    }
  }

  /**
    * @brief  LoRa发送发送hex数据包函数,也是LoRa在定点模式下执行节点间通信的主要函数。				
    * @note   hex数据包的格式为固定包长
    * @param  None
    * @retval None
    */
  void vLoRaUSART3Node1IDPkt(void)
  {
    vLoRaUSART3SendArray(&xLoRaConfig.ucLoRaNode1Addr, 2);																													
    vLoRaUSART3SendArray(&xLoRaConfig.ucLoRaNode1Channel, 1);
    vLoRaUSART3SendArray(&xLoRaConfig.ucLoRaNode1Identifier, 1);
  }

  void vLoRaUSART3Node2IDPkt(void)
  {
    vLoRaUSART3SendArray(&xLoRaConfig.ucLoRaNode2Addr, 2);																													
    vLoRaUSART3SendArray(&xLoRaConfig.ucLoRaNode2Channel, 1);
    vLoRaUSART3SendArray(&xLoRaConfig.ucLoRaNode2Identifier, 1);
  }

  void LoRa_USART3_Node1_Fan_On_Msg(void)
  {
    vLoRaUSART3Node1IDPkt();
    vLoRaUSART3SendArray(&xLoRaExecutorID.ucFan, 1);
    vLoRaUSART3SendArray(&xLoRaExecutorStatus.ucStatusOn, 1);
  }

  void LoRa_USART3_Node1_Fan_Off_Msg(void)
  {
    vLoRaUSART3Node1IDPkt();
    vLoRaUSART3SendArray(&xLoRaExecutorID.ucFan, 1);
    vLoRaUSART3SendArray(&xLoRaExecutorStatus.ucStatusOff, 1);
  }

  void LoRa_USART3_Node1_Humidifier_On_Msg(void)
  {
    vLoRaUSART3Node1IDPkt();
    vLoRaUSART3SendArray(&xLoRaExecutorID.ucHumidifier, 1);
    vLoRaUSART3SendArray(&xLoRaExecutorStatus.ucStatusOn, 1);
  }

  void LoRa_USART3_Node1_Humidifier_Off_Msg(void)
  {
    vLoRaUSART3Node1IDPkt();
    vLoRaUSART3SendArray(&xLoRaExecutorID.ucHumidifier, 1);
    vLoRaUSART3SendArray(&xLoRaExecutorStatus.ucStatusOff, 1);
  }

#else
  /*发送字符串函数*/
  void vLoRaUSART3SendString(char *String)                                                            //给uint8_t *也可以，由于字符串自带一个标志位，所以就不需要再传递长度参数了
  {
    for(uint8_t i = 0;String[i] != '\0';i++)                                                          //循环结束就可以用标志位来判断了,填'\0'是空字符的转义字符表示形式，和直接写0是一样的
    {
      vLoRaUSART3SendByte(String[i]);                                                                 //将String字符串一个个取出来，通过SendByte发送
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
    vLoRaUSART3SendByte(ch);
    return ch;
  }

  /**
    * @brief  printf封装函数					
    * @note   第一个参数是接收字符串，第二个是接收可变参数列表。
    * @param  *format,...
    * @retval None
    */
  void vLoRaUSART3Printf(char *format, ...)																													
  {
    char String[100];
    va_list arg;                                                                                      //arg是定义一个参数列表变量
    va_start(arg,format);                                                                             //从format位置开始接收参数表，放在arg里面
    vsprintf(String,format,arg);                                                                      //封装格式要用vsprintf，因为sprintf只能接收直接写的参数；打印字符串格式是format，参数表是arg，
    va_end(arg);                                                                                      //释放参数表
    vLoRaUSART3SendString(String);
  }

#endif



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
  if(USART_GetITStatus(USART3,USART_IT_RXNE) == SET)                                                  //如果RXNE置1，说明收到数据，开始根据数据处理状态。
  {
    /*接收字节，先读取到模块的变量里*/
    uint32_t rxData = USART_ReceiveData(USART3);                                                      //获取USART3接收到的数据
    switch(xLoRaUSART3Rx.ucRxState)
    {
      case 0:                                                                                         //状态1：等待包头，包头为节点号			
      {
        if (rxData == 0xD1)																					
        {
          xLoRaUSART3Rx.ucRxState = 1;
          loRaNodeID = 1;
        }
        else if (rxData == 0xD2)
        {
          xLoRaUSART3Rx.ucRxState = 1;
          loRaNodeID = 2;
        }
        break;
      }
      case 1:
      {
        if(xLoRaUSART3Rx.ucPRxPacket <= 8)                                                            //确保数组不越界
        {
          loRaUSART3RxPacket[xLoRaUSART3Rx.ucPRxPacket++] = rxData;                                   //每进一次接收状态，数据就转存一次缓存数组，同时存的位置++
        }

        switch(loRaNodeID)
        {
          case 1:
          {
            if(xLoRaUSART3Rx.ucPRxPacket == 3)
            {
              xLoRaRxJsonFlag.iTemp = loRaUSART3RxPacket[1];
              xLoRaRxJsonFlag.iHumi = loRaUSART3RxPacket[2];
              xLoRaUSART3Rx.ucRxState = 0;
              xLoRaUSART3Rx.ucPRxPacket = 0;
            }
            break;
          }
          case 2:
          {
            if(xLoRaUSART3Rx.ucPRxPacket == 8)
            {
              if(loRaUSART3RxPacket[1] == 0 && loRaUSART3RxPacket[3] == 0)
              {
                xLoRaRxJsonFlag.iSmoke = xLoRaRxJsonFlag.iFire = xLoRaRxJsonFlag.iBeep = xLoRaRxJsonFlag.iServo = 0;
              }
              else if(loRaUSART3RxPacket[1] == 0 && loRaUSART3RxPacket[3] == 1)
              {
                xLoRaRxJsonFlag.iSmoke = 0;
                xLoRaRxJsonFlag.iFire = xLoRaRxJsonFlag.iBeep = xLoRaRxJsonFlag.iServo = 1;
              }
              else if(loRaUSART3RxPacket[1] == 1 && loRaUSART3RxPacket[3] == 0)
              {
                xLoRaRxJsonFlag.iFire = 0;
                xLoRaRxJsonFlag.iSmoke = xLoRaRxJsonFlag.iBeep = xLoRaRxJsonFlag.iServo = 1;	
              }
              else if(loRaUSART3RxPacket[1] == 1 && loRaUSART3RxPacket[3] == 1)
              {
                xLoRaRxJsonFlag.iSmoke = xLoRaRxJsonFlag.iFire = xLoRaRxJsonFlag.iBeep = xLoRaRxJsonFlag.iServo = 1;
              }
              xLoRaUSART3Rx.ucRxState = 0;
              xLoRaUSART3Rx.ucPRxPacket = 0;
            }
          }
        }
        break;
      }
    }
    USART_ClearITPendingBit(USART3,USART_IT_RXNE);                                                   //if是否要清除标志位呢，如果读取了DR，就会自动清除，如果没读取就需要手动清除
  }
}
