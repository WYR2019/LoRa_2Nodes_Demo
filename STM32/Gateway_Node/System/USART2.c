#include "USART2.h"

/**
  * @brief  串口2的初始化函数         
  * @note   使用USART2串口（PB10->LoRa_RX，PB11->LoRa_TX），波特率设置为115200。
  * @param  ulUsart2Baudrate
  * @retval None
  */
void vUsart2Init(uint32_t ulUsart2Baudrate)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = USART2_GPIO_PIN_TX;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = USART2_GPIO_PIN_RX;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = ulUsart2Baudrate;                                              // 设定后，USART_Init函数内部会自动算好9600对应的分频系数，并写到BRR寄存器
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;                     // 硬件流控制,不使用流控
    USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;                                       // 串口模式，如果既使用输入和输出模式就用或符号，发送模式和接收模式
    USART_InitStructure.USART_Parity = USART_Parity_No;                                                 // 无校验位
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                                              // 一位停止位
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                                         // 字长，不需要校验，字长就选择8位
    USART_Init(USART2,&USART_InitStructure);
                                                                                                        // 上面是串口的查询模式，如果使用中断，还需要开启中断，配置NVIC
    USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);                                                        // 选择RXNE的中断,并开启RXNE标志位到NVIC的输出，如果RXNE标志位置1，就会向NVIC申请中断，之后可以在中断函数中接收数据

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);                                                     // 分组，分组4为4bit抢占优先级和0bit响应优先级，抢占优先级可设置为0-15。由于FreeRTOS没有响应优先级，而优先级分组4相当于4位抢占优先级和0位响应优先级。
    NVIC_InitTypeDef NVIC_InitStructure;                                                                // 初始化NVIC的USART1通道
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;                                                   // 中断通道
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;                                           // 抢占优先级配置，根据FreeRTOS设置配置，在范围内可以调用FreeRTOS的以“FromISR()”结尾的api函数。
    // NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;                                               // 响应优先级配置，用不上了
    NVIC_Init(&NVIC_InitStructure);                                                                     // 指向NVIC_InitStructure的地址

    USART_Cmd(USART2,ENABLE);
}

/**
  * @brief  串口发送单个字节函数
  * @note   调用这个函数，就可以从TX引脚发送一个字节数据。
  * @param  ucByte
  * @retval None
  */
void vUsart2SendByte(uint8_t ucByte)
{
    USART_SendData(USART2, ucByte);                                                                     // 调用这个函数，Byte就写入TDR寄存器了
                                                                                                        // 写完之后还需要等待一下，等TDR的数据转移到移位寄存器就可以了，如果数据还在TDR寄存器中，再写入数据就会产生数据覆盖，所以在发送之后还需要等待一下标志位
    while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);                                        // 发送数据寄存器空标志位，等待TXE置1，所以需要套一个while循环 TXE:发送数据寄存器空，发送完标志位自动置0，不用手动复位。
}

/**
  * @brief  串口发送数组函数
  * @note   一般用于16进制模式下。
  * @note	  uint8_t的指针类型，指向待发送数组的首地址,由于数组无法判断是否结束，所以需要再传递一个Length进来。
  * @param  *pucArray，usLength
  * @retval None
  */
void vUsart2SendArray(uint8_t *pucArray, uint16_t usLength)																					
{
    for(uint16_t i = 0; i < usLength; i ++)                                                             // for循环执行Length次，可以对Array数据进行遍历，实际定义数组不要超出uint16_t的范围即可
    {
        vUsart2SendByte(pucArray[i]);                                                                   // 依次取出数组Array的每一项
    }
}

/**
  * @brief  串口发送字符串函数
  * @note   将字符串封装成指针数组的形式，定义数组长度直到检测到字符串的末尾结束，并将字符串根据长度拆成带个字符发送。
  * @param  *pcString
  * @retval None
  */
void vUsart2SendString(char *pcString)                                                                  // 给uint8_t *也可以，由于字符串自带一个标志位，所以就不需要再传递长度参数了
{
    for(uint8_t i = 0; pcString[i] != '\0'; i++)                                                        // 循环结束就可以用标志位来判断了,填'\0'是空字符的转义字符表示形式，和直接写0是一样的
    {
        vUsart2SendByte(pcString[i]);                                                                   // 将String字符串一个个取出来，通过SendByte发送
    }
}

/**
  * @brief  printf重定向函数
  * @note   fputc是printf函数的底层，printf函数在打印的时候，就是不断调用fputc函数打印。
  * @param  ch,*f
  * @retval ch
  */
int fputc(int ch, FILE *f)
{
    vUsart2SendByte(ch);
    return ch;
}

/**
  * @brief  printf封装函数
  * @note   第一个参数是接收字符串，第二个是接收可变参数列表。
  * @param  *format,...
  * @retval None
  */
void vUsart2Printf(char *format, ...)
{
    char String[100];
    va_list arg;                                                                                        // arg是定义一个参数列表变量
    va_start(arg, format);                                                                              // 从format位置开始接收参数表，放在arg里面
    vsprintf(String, format, arg);                                                                      // 封装格式要用vsprintf，因为sprintf只能接收直接写的参数；打印字符串格式是format，参数表是arg，
    va_end(arg);                                                                                        // 释放参数表
    vUsart2SendString(String);
}

void USART2_IRQHandler(void)
{
    if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
    {
        /* code */
        uint8_t ulRxData = (uint8_t)USART_ReceiveData(USART2);
        #if (USE_NONE_SYSTEM == 1)
        #elif (USE_FREERTOS == 1)
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            xQueueSendFromISR(xQueueUsart2IrqHdlr, &ulRxData, &xHigherPriorityTaskWoken);
            /* 问题根源：请求上下文切换 */ 
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        #elif (USE_OSAL == 1)
        #endif
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    } 
}
