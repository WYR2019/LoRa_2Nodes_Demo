#include "BSP_USART.h"

/**
  * @brief  串口1的初始化函数         
  * @note   使用USART1串口（PA9->USART1_RX，PA10->USART1_TX），波特率设置为115200。
  * @param  *usartId  选择串口号
  * @param  baudrate 要设置的波特率
  * @retval None
  */
void usartInit(USART_TypeDef *usartId, uint32_t baudrate)
{
    if (usartId == USART1)
    {
        RCC_APB2PeriphClockCmd(USART1_GPIO_CLOCK, ENABLE);

        GPIO_InitTypeDef GPIO_InitStructure;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Pin = USART1_GPIO_PIN_TX;
        GPIO_Init(USART1_GPIO_PORT, &GPIO_InitStructure);
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Pin = USART1_GPIO_PIN_RX;
        GPIO_Init(USART1_GPIO_PORT, &GPIO_InitStructure);

        USART_InitTypeDef USART_InitStructure;
        USART_InitStructure.USART_BaudRate = baudrate;                                              // 设定后，USART_Init函数内部会自动算好9600对应的分频系数，并写到BRR寄存器
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;             // 硬件流控制,不使用流控
        USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;                               // 串口模式，如果既使用输入和输出模式就用或符号，发送模式和接收模式
        USART_InitStructure.USART_Parity = USART_Parity_No;                                         // 无校验位
        USART_InitStructure.USART_StopBits = USART_StopBits_1;                                      // 一位停止位
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;                                 // 字长，不需要校验，字长就选择8位
        USART_Init(USART1, &USART_InitStructure);
                                                                                                    // 上面是串口的查询模式，如果使用中断，还需要开启中断，配置NVIC
        USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);                                             // 选择RXNE的中断,并开启RXNE标志位到NVIC的输出，如果RXNE标志位置1，就会向NVIC申请中断，之后可以在中断函数中接收数据
        USART_ITConfig(USART1, USART_IT_IDLE, DISABLE);                                             // 使能串口总线空闲中断
        NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);                                             // 分组，分组4为4bit抢占优先级和0bit响应优先级，抢占优先级可设置为0-15。由于FreeRTOS没有响应优先级，而优先级分组4相当于4位抢占优先级和0位响应优先级。
        NVIC_InitTypeDef NVIC_InitStructure;                                                        // 初始化NVIC的USART1通道
        NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;                                           // 中断通道
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;                                   // 抢占优先级配置，根据FreeRTOS设置配置，在范围内可以调用FreeRTOS的以“FromISR()”结尾的api函数。
        // NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;                                       // 响应优先级配置，用不上了
        NVIC_Init(&NVIC_InitStructure);                                                             // 指向NVIC_InitStructure的地址

        USART_Cmd(USART1, ENABLE);
    } else if (usartId == USART2)
    {
        RCC_APB2PeriphClockCmd(USART2_GPIO_CLOCK, ENABLE);
        GPIO_InitTypeDef GPIO_InitStructure;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Pin = USART2_GPIO_PIN_TX;
        GPIO_Init(USART2_GPIO_PORT, &GPIO_InitStructure);
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
        GPIO_InitStructure.GPIO_Pin = USART2_GPIO_PIN_RX;
        GPIO_Init(USART2_GPIO_PORT, &GPIO_InitStructure);

        USART_InitTypeDef USART_InitStructure;
        USART_InitStructure.USART_BaudRate = baudrate;                                              // 设定后，USART_Init函数内部会自动算好9600对应的分频系数，并写到BRR寄存器
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;             // 硬件流控制,不使用流控
        USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;                               // 串口模式，如果既使用输入和输出模式就用或符号，发送模式和接收模式
        USART_InitStructure.USART_Parity = USART_Parity_No;                                         // 无校验位
        USART_InitStructure.USART_StopBits = USART_StopBits_1;                                      // 一位停止位
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;                                 // 字长，不需要校验，字长就选择8位
        USART_Init(USART2, &USART_InitStructure);
                                                                                                    // 上面是串口的查询模式，如果使用中断，还需要开启中断，配置NVIC
        USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);                                              // 选择RXNE的中断,并开启RXNE标志位到NVIC的输出，如果RXNE标志位置1，就会向NVIC申请中断，之后可以在中断函数中接收数据
        USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);                                              // 使能串口总线空闲中断

        NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);                                             // 分组，分组4为4bit抢占优先级和0bit响应优先级，抢占优先级可设置为0-15。由于FreeRTOS没有响应优先级，而优先级分组4相当于4位抢占优先级和0位响应优先级。
        NVIC_InitTypeDef NVIC_InitStructure;                                                        // 初始化NVIC的USART1通道
        NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;                                           // 中断通道
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;                                   // 抢占优先级配置，根据FreeRTOS设置配置，在范围内可以调用FreeRTOS的以“FromISR()”结尾的api函数。
        // NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;                                       // 响应优先级配置，用不上了
        NVIC_Init(&NVIC_InitStructure);                                                             // 指向NVIC_InitStructure的地址

        USART_Cmd(USART2, ENABLE);
    } else if (usartId == USART3)
    {
        RCC_APB2PeriphClockCmd(USART3_GPIO_CLOCK, ENABLE);

        GPIO_InitTypeDef GPIO_InitStructure;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Pin = USART3_GPIO_PIN_TX;
        GPIO_Init(USART3_GPIO_PORT, &GPIO_InitStructure);
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
        GPIO_InitStructure.GPIO_Pin = USART3_GPIO_PIN_RX;
        GPIO_Init(USART3_GPIO_PORT, &GPIO_InitStructure);

        USART_InitTypeDef USART_InitStructure;
        USART_InitStructure.USART_BaudRate = baudrate;                                              // 设定后，USART_Init函数内部会自动算好9600对应的分频系数，并写到BRR寄存器
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;             // 硬件流控制,不使用流控
        USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;                               // 串口模式，如果既使用输入和输出模式就用或符号，发送模式和接收模式
        USART_InitStructure.USART_Parity = USART_Parity_No;                                         // 无校验位
        USART_InitStructure.USART_StopBits = USART_StopBits_1;                                      // 一位停止位
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;                                 // 字长，不需要校验，字长就选择8位
        USART_Init(USART3, &USART_InitStructure);
                                                                                                    // 上面是串口的查询模式，如果使用中断，还需要开启中断，配置NVIC
        USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);                                              // 选择RXNE的中断,并开启RXNE标志位到NVIC的输出，如果RXNE标志位置1，就会向NVIC申请中断，之后可以在中断函数中接收数据
        USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);                                              // 使能串口总线空闲中断    
        NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);                                             // 分组，分组4为4bit抢占优先级和0bit响应优先级，抢占优先级可设置为0-15。由于FreeRTOS没有响应优先级，而优先级分组4相当于4位抢占优先级和0位响应优先级。
        NVIC_InitTypeDef NVIC_InitStructure;                                                        // 初始化NVIC的USART1通道
        NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;                                           // 中断通道
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;                                   // 抢占优先级配置，根据FreeRTOS设置配置，在范围内可以调用FreeRTOS的以“FromISR()”结尾的api函数。
        // NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;                                       // 响应优先级配置，用不上了
        NVIC_Init(&NVIC_InitStructure);                                                             // 指向NVIC_InitStructure的地址

        USART_Cmd(USART3, ENABLE);
    }
}

/**
  * @brief  串口发送单个字节函数
  * @note   调用这个函数，就可以从TX引脚发送一个字节数据。
  * @param  *usartId 选择串口号
  * @param  byte    要发送的字节
  * @retval None
  */
void usartSendByte(USART_TypeDef *usartId, uint8_t byte)
{
    USART_SendData(usartId, byte);                                                                  // 调用这个函数，Byte就写入TDR寄存器了
                                                                                                    // 写完之后还需要等待一下，等TDR的数据转移到移位寄存器就可以了，如果数据还在TDR寄存器中，再写入数据就会产生数据覆盖，所以在发送之后还需要等待一下标志位
    while(USART_GetFlagStatus(usartId, USART_FLAG_TXE) == RESET);                                   // 发送数据寄存器空标志位，等待TXE置1，所以需要套一个while循环 TXE:发送数据寄存器空，发送完标志位自动置0，不用手动复位。
}

/**
  * @brief  串口发送数组函数
  * @note   一般用于16进制模式下。uint8_t的指针类型，指向待发送数组的首地址,由于数组无法判断是否结束，所以需要再传递一个Length进来。
  * @param  *usartId 选择串口号
  * @param  *array 要发送的十六进制数字
  * @param  length  发送长度
  * @retval None
  */
void usartSendArray(USART_TypeDef *usartId, uint8_t *array, uint16_t length)
{
    for(uint16_t i = 0; i < length; i ++)                                                           // for循环执行Length次，可以对Array数据进行遍历，实际定义数组不要超出uint16_t的范围即可
    {
        usartSendByte(usartId, array[i]);                                                           // 依次取出数组Array的每一项
        while(USART_GetFlagStatus(usartId, USART_FLAG_TXE) == RESET);
    } 
}

/**
  * @brief  串口发送字符串函数
  * @note   将字符串封装成指针数组的形式，定义数组长度直到检测到字符串的末尾结束，并将字符串根据长度拆成单个字符发送。
  * @param  *usartId 选择串口号
  * @param  *string 要发送的数据
  * @retval None
  */
void usartSendString(USART_TypeDef *usartId, uint8_t *string)                                       // 给uint8_t *也可以，由于字符串自带一个标志位，所以就不需要再传递长度参数了
{
    if (string == NULL)
    {
        return;
    }
    // while (*string != '\0')
    // {
    //     usartSendByte(usartId, *string);
    //     string++;
    // }
    for (uint8_t i = 0; string[i] != '\0'; i++)
    {
        usartSendByte(usartId, string[i]);
    }
}

/**
  * @brief  串口打印函数
  * @note   将字符串封装成指针数组的形式，定义数组长度直到检测到字符串的末尾结束，并将字符串根据长度拆成带个字符发送。
  * @param  *usartId 选择串口号
  * @param  *format   字符串
  * @param  ...       可变参数列表
  * @retval None
  */
void usartPrintf(USART_TypeDef *usartId, char *format, ...)
{
    uint8_t buffer[256] = {0};                                                                      // 定义一个缓冲区，存放格式化后的字符串
    va_list arg;                                                                                    // arg是定义一个参数列表变量
    va_start(arg, format);                                                                          // 从format位置开始接收参数表，放在arg里面
    vsnprintf((char *)buffer, sizeof(buffer), format, arg);                                         // 封装格式要用vsprintf，因为sprintf只能接收直接写的参数；打印字符串格式是format，参数表是arg，
    va_end(arg);                                                                                    // 释放参数表
    uint8_t *p = buffer;
    if (usartId == USART1)
    {
        while (*p)
        {
            while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);                            // 等待发送缓冲区为空
            USART_SendData(USART1, *p++);
        }
    } 
    else if (usartId == USART2)
    {
        while (*p)
        {
            while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
            USART_SendData(USART2, *p++);
        }
    } 
    else if (usartId == USART3)
    {
        while (*p)
        {
            while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
            USART_SendData(USART3, *p++);
        }
    }
}

/**
  * @brief  串口1接收中断处理函数
  * @note   接收串口中断发送的每帧数据到串口消息队列中，以便后续处理数据。
  * @param  None
  * @retval None
  */
void USART1_IRQHandler(void)
{
#if ENABLE_FREERTOS_API_QUEUE_USART1_IRQ
    if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
    {
        uint8_t rxData = (uint8_t)USART_ReceiveData(USART1);
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xQueueSendFromISR(xQueueUsart1IrqHdlr, &rxData, &xHigherPriorityTaskWoken);
        /* 问题根源：请求上下文切换 */ 
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
#else

#endif
}
