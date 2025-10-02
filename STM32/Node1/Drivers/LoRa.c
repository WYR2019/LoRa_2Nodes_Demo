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
  * 1��������ΪLoRa���ݲɼ��ڵ�1��������������STM32F103C8T6�����£�֧��3��ģʽ��
  * �ֱ�Ϊ͸��ģʽ������ģʽ�ͳ�ʼ��ģʽ��ͨ���궨���ʼ��LoRaʹ��ģʽ��0Ϊ��ʼ
  * ��ģʽ��1Ϊ����ģʽ��2Ϊ͸��ģʽ��Ŀǰ����ɶ���ģʽ����Ĵ����д��
  * 2��LoRa���ݲɼ��ڵ�1��Ҫʹ��DHT11�������ɼ���ʪ�ȣ��û������ƶ���Զ�̿��Ʒ��ȡ�
  * LED�ƺͼ�ʪ����
  * 3������ģʽ��16�������ݰ���ʽ�������ݣ���modbusЭ����д��䡣
  * ʾ����03 E9 17 D1 EA 17 45
  * ���У�03 E9ΪLoRa���ص�ַ��16�������֣�3E9תΪ10����Ϊ1001��17ΪLoRa�����ŵ���16
  * �������֣�ת10����Ϊ23��D1Ϊ�ڵ�1�ı�ʶ��EAΪDHT11�������ı�ʶ��17��45�ֱ�Ϊ��ʪ
  * �ȵ�16�������֡�
  ******************************************************************************
  */

static uint8_t ucLoRaUSART3RxPacket[3];

/**
  * @brief  LoRa�ĳ�ʼ������         
  * @note   ʹ��USART3���ڣ�PB10->LoRa_RX��PB11->LoRa_TX��������������Ϊ115200��
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
  USART_InitStructure.USART_BaudRate = ulLoRaUSART3Baudrate;                                        //�趨��USART_Init�����ڲ����Զ����9600��Ӧ�ķ�Ƶϵ������д��BRR�Ĵ���
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;                   //Ӳ��������,��ʹ������
  USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;                                     //����ģʽ�������ʹ����������ģʽ���û���ţ�����ģʽ�ͽ���ģʽ
  USART_InitStructure.USART_Parity = USART_Parity_No;                                               //��У��λ
  USART_InitStructure.USART_StopBits = USART_StopBits_1;                                            //һλֹͣλ
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;                                       //�ֳ�������ҪУ�飬�ֳ���ѡ��8λ
  USART_Init(USART3,&USART_InitStructure);
                                                                                                    //�����Ǵ��ڵĲ�ѯģʽ�����ʹ���жϣ�����Ҫ�����жϣ�����NVIC
  USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);                                                      //ѡ��RXNE���ж�,������RXNE��־λ��NVIC����������RXNE��־λ��1���ͻ���NVIC�����жϣ�֮��������жϺ����н�������

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);                                                   //����
  NVIC_InitTypeDef NVIC_InitStructure;                                                              //��ʼ��NVIC��USART1ͨ��
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;                                                 //�ж�ͨ��
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_Init(&NVIC_InitStructure);                                                                   //ָ��NVIC_InitStructure�ĵ�ַ

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
  * @brief  LoRa���͵����ֽں���
  * @note   ��������������Ϳ��Դ�TX���ŷ���һ���ֽ����ݡ�
  * @param  byte
  * @retval None
  */
void vLoRaUSART3SendByte(uint8_t ucByte)
{
  USART_SendData(USART3,ucByte);                                                                      //�������������Byte��д��TDR�Ĵ�����
                                                                                                      //д��֮����Ҫ�ȴ�һ�£���TDR������ת�Ƶ���λ�Ĵ����Ϳ����ˣ�������ݻ���TDR�Ĵ����У���д�����ݾͻ�������ݸ��ǣ������ڷ���֮����Ҫ�ȴ�һ�±�־λ
  while(USART_GetFlagStatus(USART3,USART_FLAG_TXE) == RESET);                                         //�������ݼĴ����ձ�־λ���ȴ�TXE��1��������Ҫ��һ��whileѭ�� TXE:�������ݼĴ����գ��������־λ�Զ���0�������ֶ���λ
}

#if LORA_MODE_INIT
  /**
    * @brief  LoRa�������麯��
    * @note   һ������16����ģʽ�¡�
    * @note	  uint8_t��ָ�����ͣ�ָ�������������׵�ַ,���������޷��ж��Ƿ������������Ҫ�ٴ���һ��Length������
    * @param  *array��length
    * @retval None
    */
  void vLoRaUSART3SendArray(uint8_t *pucArray,uint16_t usLength)																					
  {
    for(uint16_t i = 0;i < usLength;i ++)                                                             //forѭ��ִ��Length�Σ����Զ�Array���ݽ��б�����ʵ�ʶ������鲻Ҫ����uint16_t�ķ�Χ����
    {
      vLoRaUSART3SendByte(pucArray[i]);                                                               //����ȡ������Array��ÿһ��
    }
  }
	
  /**
    * @brief  LoRa���ͷ���hex���ݰ�����,Ҳ��LoRa�ڶ���ģʽ��ִ�нڵ��ͨ�ŵ���Ҫ������				
    * @note   hex���ݰ��ĸ�ʽΪ�̶�����
    * @param  None
    * @retval None
    */
  void vLoRaUSART3GateIdPkt(void)
  {
    vLoRaUSART3SendArray(&xLoRaConfig.ucLoRaGateAddr, 2);                                             //�����غ�
    vLoRaUSART3SendArray(&xLoRaConfig.ucLoRaChannel, 1);
    vLoRaUSART3SendArray(&xLoRaConfig.ucLoRaIdentifier, 1);
  }
#else
  /*�����ַ�������*/
  void vLoRaUSART3SendString(char *pcString)                                                          //��uint8_t *Ҳ���ԣ������ַ����Դ�һ����־λ�����ԾͲ���Ҫ�ٴ��ݳ��Ȳ�����
  {
    for(uint8_t i = 0;string[i] != '\0';i++)                                                          //ѭ�������Ϳ����ñ�־λ���ж���,��'\0'�ǿ��ַ���ת���ַ���ʾ��ʽ����ֱ��д0��һ����
    {
      vLoRaUSART3SendByte(pcString[i]);                                                               //��String�ַ���һ����ȡ������ͨ��SendByte����
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
    vLoRaUSART3SendByte(ch);
    return ch;
  }

  /**
    * @brief  printf��װ����					
    * @note   ��һ�������ǽ����ַ������ڶ����ǽ��տɱ�����б�
    * @param  *format,...
    * @retval None
    */
  void vLoRaUSART3Printf(char *format, ...)                        																												
  {
    char String[100];
    va_list arg;                                                                                      //arg�Ƕ���һ�������б����
    va_start(arg,format);                                                                             //��formatλ�ÿ�ʼ���ղ���������arg����
    vsprintf(String,format,arg);                                                                      //��װ��ʽҪ��vsprintf����Ϊsprintfֻ�ܽ���ֱ��д�Ĳ�������ӡ�ַ�����ʽ��format����������arg��
    va_end(arg);                                                                                      //�ͷŲ�����
    vLoRaUSART3SendString(String);
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
  LoRaExecutorFlag_t xLoRaExecutorFlag;

  if(USART_GetITStatus(USART3,USART_IT_RXNE) == SET)                                                  //���RXNE��1��˵���յ����ݣ���ʼ�������ݴ���״̬��
  {
    /*�����ֽڣ��ȶ�ȡ��ģ��ı�����*/
    uint32_t ucRxData = USART_ReceiveData(USART3);                                                    //��ȡUSART3���յ�������

    #if LORA_MODE_INIT == 0
      functions();

    #elif LORA_MODE_INIT
      if(xLoRaUSART3Rx.ucRxState == 0)
      {
        if(ucRxData == 0xD1)                                                                          //�жϰ�ͷ�Ƿ���,�Ƿ��յ�0xD1�����յ���������ݴ���״̬��
        {
          xLoRaUSART3Rx.ucRxState = 1;
        }
      }
      else if(xLoRaUSART3Rx.ucRxState == 1)
      {
        ucLoRaUSART3RxPacket[xLoRaUSART3Rx.ucPRxPacket++] = ucRxData;                                 //��pRxPacket�����ݸ�ֵ��rxData����rxData�浽���������ÿ��һ�ν���״̬�����ݾ�ת��һ�ν������飬ͬʱ���λ��++,Ų����һ��λ�á�
        switch(xLoRaUSART3Rx.ucPRxPacket)
        {
          case 1:
            switch(ucLoRaUSART3RxPacket[0])
            {
              case 0xFA:xLoRaExecutorFlag.ucId = 10;break;
              case 0xFB:xLoRaExecutorFlag.ucId = 20;break;
              case 0xFC:xLoRaExecutorFlag.ucId = 30;break;
            }
          case 2:
          switch(xLoRaExecutorFlag.ucId)
          {
            case 10:
            {
              xLoRaExecutorFlag.ucState = (ucLoRaUSART3RxPacket[1] == 0x01) ? 11 : 12;
              break;
            }
            case 20:
            {
              xLoRaExecutorFlag.ucState = (ucLoRaUSART3RxPacket[1] == 0x01) ? 21 : 22;
              break;
            }
            case 30:
            {
              xLoRaExecutorFlag.ucState = (ucLoRaUSART3RxPacket[1] == 0x01) ? 31 : 32;
              break;
            }
          }
          /* DeepSeek�޸Ĳ��� */
          xLoRaUSART3Rx.ucRxState = 0;
          xLoRaUSART3Rx.ucPRxPacket = 0;

          default:
          {
		  			xLoRaUSART3Rx.ucRxState = 0;
            xLoRaUSART3Rx.ucPRxPacket = 0;
          }
        }
      }

    #elif LORA_MODE_INIT == 2
      functions();

    #endif

    USART_ClearITPendingBit(USART3,USART_IT_RXNE);                                                     //if�Ƿ�Ҫ�����־λ�أ������ȡ��DR���ͻ��Զ���������û��ȡ����Ҫ�ֶ����
  }
}
