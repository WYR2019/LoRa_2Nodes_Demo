#include "LoRa.h"

/**
  ******************************************************************************
  * @file    LoRa.c
  * @author  github/WYR2019
  * @version V1.0
  * @date    2025/5/25
  * @brief   LoRa�ڵ�1����
  ******************************************************************************
  * @attention
  * 1��������ΪLoRa���ݲɼ��ڵ�1��������������STM32F103C8T6�����£�֧��3��ģʽ���ֱ�
  * Ϊ͸��ģʽ������ģʽ�ͳ�ʼ��ģʽ��ͨ���궨���ʼ��LoRaʹ��ģʽ��0Ϊ��ʼ��ģʽ��1Ϊ
	* ����ģʽ��2Ϊ͸��ģʽ��Ŀǰ����ɶ���ģʽ����Ĵ����д��
	* 2��LoRa���ݲɼ��ڵ�1��Ҫʹ��DHT11�������ɼ���ʪ�ȣ��û������ƶ���Զ�̿��Ʒ��ȡ�
  * LED�ƺͼ�ʪ����
  * 3������ģʽ��16�������ݰ���ʽ�������ݣ���modbusЭ����д��䡣
  * ʾ����03 E9 17 D1 EA 17 45
  * ���У�03 E9ΪLoRa���ص�ַ��16�������֣�3E9תΪ10����Ϊ1001��17ΪLoRa�����ŵ���16
  * �������֣�ת10����Ϊ23��D1Ϊ�ڵ�1�ı�ʶ��EAΪDHT11�������ı�ʶ��17��45�ֱ�Ϊ��ʪ
  * �ȵ�16�������֡�
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
  * @brief  LoRa�ڴ���ģʽ�µĳ�ʼ������         
  * @note   ʹ��USART3���ڣ�PB10->LoRa_RX��PB11->LoRa_TX��������������Ϊ9600��
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
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;																											//��������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Pin = LORA_GPIO_PIN_RX;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
  USART_InitTypeDef USART_InitStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
  USART_InitStructure.USART_BaudRate = 9600;																							          //9600�����ʣ�д���USART_Init�����ڲ����Զ����9600��Ӧ�ķ�Ƶϵ������д��BRR�Ĵ���
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

  #elif LORA_MODE_INIT == 2
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
  * @brief  LoRa���͵����ֽں���
  * @note   ��������������Ϳ��Դ�TX���ŷ���һ���ֽ����ݡ�
  * @param  byte
  * @retval None
  */
void LoRa_USART3_SendByte(uint8_t byte)
{
  USART_SendData(USART3,byte);                                                                      //�������������Byte��д��TDR�Ĵ�����
                                                                                                    //д��֮����Ҫ�ȴ�һ�£���TDR������ת�Ƶ���λ�Ĵ����Ϳ����ˣ�������ݻ���TDR�Ĵ����У���д�����ݾͻ�������ݸ��ǣ������ڷ���֮����Ҫ�ȴ�һ�±�־λ
  while(USART_GetFlagStatus(USART3,USART_FLAG_TXE) == RESET);                                       //�������ݼĴ����ձ�־λ���ȴ�TXE��1��������Ҫ��һ��whileѭ�� TXE:�������ݼĴ����գ��������־λ�Զ���0�������ֶ���λ
}

#if LORA_MODE_INIT
/**
  * @brief  LoRa�������麯��
  * @note   һ������16����ģʽ�¡�
  * @note	  uint8_t��ָ�����ͣ�ָ�������������׵�ַ,���������޷��ж��Ƿ������������Ҫ�ٴ���һ��Length������
  * @param  *array��length
  * @retval None
  */
void LoRa_USART3_SendArray(uint8_t *array,uint16_t length)																					
{
  for(uint16_t i = 0;i < length;i ++)                                                               //forѭ��ִ��Length�Σ����Զ�Array���ݽ��б�����ʵ�ʶ������鲻Ҫ����uint16_t�ķ�Χ����
  {
    LoRa_USART3_SendByte(array[i]);                                                                 //����ȡ������Array��ÿһ��
  }
}

#else
/*�����ַ�������*/
void LoRa_USART3_SendString(char *string)                                                           //��uint8_t *Ҳ���ԣ������ַ����Դ�һ����־λ�����ԾͲ���Ҫ�ٴ��ݳ��Ȳ�����
{
  for(uint8_t i = 0;string[i] != '\0';i++)                                                          //ѭ�������Ϳ����ñ�־λ���ж���,��'\0'�ǿ��ַ���ת���ַ���ʾ��ʽ����ֱ��д0��һ����
  {
    LoRa_USART3_SendByte(string[i]);                                                                //��String�ַ���һ����ȡ������ͨ��SendByte����
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
  va_list arg;                                                                                      //arg�Ƕ���һ�������б����
  va_start(arg,format);                                                                             //��formatλ�ÿ�ʼ���ղ���������arg����
  vsprintf(String,format,arg);                                                                      //��װ��ʽҪ��vsprintf����Ϊsprintfֻ�ܽ���ֱ��д�Ĳ�������ӡ�ַ�����ʽ��format����������arg��
  va_end(arg);                                                                                      //�ͷŲ�����
  LoRa_USART3_SendString(String);
}
#endif

#if LORA_MODE_INIT
/**
  * @brief  LoRa���ͷ���hex���ݰ�����,Ҳ��LoRa�ڶ���ģʽ��ִ�нڵ��ͨ�ŵ���Ҫ������				
  * @note   hex���ݰ��ĸ�ʽΪ�̶�����
  * @param  None
  * @retval None
  */
void LoRa_USART3_Gate_IdPkt(void)
{
  LoRa_USART3_SendArray(loRaGateAddr, 2);                                                           //�����غ�
  LoRa_USART3_SendArray(loRaGateChannel,1);
  LoRa_USART3_SendArray(loRaGateIdentifier,1);
}



void LoRa_USART3_Send_LED_On_Status(void)
{
  
}
#endif

/**
  * @brief  USART3�����жϺ���
  * @note   1������USART3���յ������ݣ�ִ����Ӧ������
  *         2��״̬����һ����Ϊ3�����ֱ��ǵȴ���ͷ��0xD1�����������ݺ͵ȴ���β��0xAB����
  *         3��else if������ɶ��״̬������������⡣
  *         4���ж�ֻ��ִ�е�һ������������if���
  *         5��exeState�����µ�״̬����LED�ƣ�10��ʾ�յ����ص�LED����11��ʾ��LED��12��ʾ�ر�LED��
  * @param  None
  * @retval None
  */
void USART3_IRQHandler(void)
{
  /*״̬����һ����Ϊ3�����ֱ���0��1��2��Ҳ���ǵȴ���ͷ������ִ�������ݡ�����ִ����״̬���ݺ͵ȴ���β*/
  static uint8_t rxState = 0;                                                                       //״̬����S=0
  static uint8_t pRxPacket = 0;                                                                     //ָʾ���յ���һ������
	
  if(USART_GetITStatus(USART3,USART_IT_RXNE) == SET)                                                //���RXNE��1��˵���յ����ݣ���ʼ�������ݴ���״̬��
  {
    /*�����ֽڣ��ȶ�ȡ��ģ��ı�����*/
    uint32_t rxData = USART_ReceiveData(USART3);                                                    //��ȡUSART3���յ�������
		
    #if LORA_MODE_INIT == 0
		
    #elif LORA_MODE_INIT
    switch(rxState)
    {
      case 0:                                                                                       //�ж��Ƿ��յ�0xD1�����յ���������ݴ���״̬��
			{
        if(rxData == 0xD1)                                                                          //�жϰ�ͷ�Ƿ���ȷ
        {
          rxState = 1;
        }
        break;
			}
      case 1:
      {
        loRaUSART3RxPacket[pRxPacket++]	= rxData;                                                   //��pRxPacket�����ݸ�ֵ��rxData����rxData�浽���������ÿ��һ�ν���״̬�����ݾ�ת��һ�ν������飬ͬʱ���λ��++,Ų����һ��λ�á�
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
          /* DeepSeek�޸Ĳ��� */
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
		
    USART_ClearITPendingBit(USART3,USART_IT_RXNE);                                                //if�Ƿ�Ҫ�����־λ�أ������ȡ��DR���ͻ��Զ���������û��ȡ����Ҫ�ֶ����
  }
}
