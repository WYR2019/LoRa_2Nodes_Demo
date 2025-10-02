#include "LoRa_Gateway.h"

uint8_t loRaUSART3RxPacket[80];

uint8_t loRaNodeID, sensorID;

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
  * @param  Byte
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
  /*�����ַ�������*/
  void vLoRaUSART3SendString(char *String)                                                            //��uint8_t *Ҳ���ԣ������ַ����Դ�һ����־λ�����ԾͲ���Ҫ�ٴ��ݳ��Ȳ�����
  {
    for(uint8_t i = 0;String[i] != '\0';i++)                                                          //ѭ�������Ϳ����ñ�־λ���ж���,��'\0'�ǿ��ַ���ת���ַ���ʾ��ʽ����ֱ��д0��һ����
    {
      vLoRaUSART3SendByte(String[i]);                                                                 //��String�ַ���һ����ȡ������ͨ��SendByte����
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
  * @brief  USART3�����жϺ�����״̬�� 
  * @note   ����USART3���յ������ݣ�ִ�ж�Ӧ������
  * @note   ״̬����һ����Ϊ3�����ֱ���0��1��2��Ҳ���ǵȴ���ͷ���������ݺ͵ȴ���β��
  * @note   else if������ɶ��״̬������������⡣
  * @param  None
  * @retval None
  */
void USART3_IRQHandler(void)
{	
  if(USART_GetITStatus(USART3,USART_IT_RXNE) == SET)                                                  //���RXNE��1��˵���յ����ݣ���ʼ�������ݴ���״̬��
  {
    /*�����ֽڣ��ȶ�ȡ��ģ��ı�����*/
    uint32_t rxData = USART_ReceiveData(USART3);                                                      //��ȡUSART3���յ�������
    switch(xLoRaUSART3Rx.ucRxState)
    {
      case 0:                                                                                         //״̬1���ȴ���ͷ����ͷΪ�ڵ��			
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
        if(xLoRaUSART3Rx.ucPRxPacket <= 8)                                                            //ȷ�����鲻Խ��
        {
          loRaUSART3RxPacket[xLoRaUSART3Rx.ucPRxPacket++] = rxData;                                   //ÿ��һ�ν���״̬�����ݾ�ת��һ�λ������飬ͬʱ���λ��++
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
    USART_ClearITPendingBit(USART3,USART_IT_RXNE);                                                   //if�Ƿ�Ҫ�����־λ�أ������ȡ��DR���ͻ��Զ���������û��ȡ����Ҫ�ֶ����
  }
}
