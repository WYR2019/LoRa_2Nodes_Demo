#include "ESP8266.h"

uint8_t esp8266RxFlag;
char esp8266RxPacket[100];//ֻ�洢���ͻ���յ��غ�����
uint8_t esp8266ConFinishState = 0;


void ESP8266_USART2_Init(uint32_t BaudRate)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = ESP8266_GPIO_PIN_TX;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;																											//��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = ESP8266_GPIO_PIN_RX;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	USART_InitStructure.USART_BaudRate = BaudRate;																										//9600�����ʣ�д���USART_Init�����ڲ����Զ����9600��Ӧ�ķ�Ƶϵ������д��BRR�Ĵ���
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;										//Ӳ��������,��ʹ������
	USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;																			//����ģʽ�������ʹ����������ģʽ���û���ţ�����ģʽ�ͽ���ģʽ
	USART_InitStructure.USART_Parity = USART_Parity_No;																								//��У��λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;																						//һλֹͣλ
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;																				//�ֳ�������ҪУ�飬�ֳ���ѡ��8λ
	USART_Init(USART2,&USART_InitStructure);
																																																		//�����Ǵ��ڵĲ�ѯģʽ�����ʹ���жϣ�����Ҫ�����жϣ�����NVIC
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);																											//ѡ��RXNE���ж�,������RXNE��־λ��NVIC����������RXNE��־λ��1���ͻ���NVIC�����жϣ�֮��������жϺ����н�������
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);																										//����
	NVIC_InitTypeDef NVIC_InitStructure;																															//��ʼ��NVIC��USART2ͨ��
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;																									//�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);																																		//ָ��NVIC_InitStructure�ĵ�ַ
	USART_Cmd(USART2,ENABLE);
}

/**
  * @brief  LoRa���͵����ֽں���
  * @note   ��������������Ϳ��Դ�TX���ŷ���һ���ֽ����ݡ�
  * @param  Byte
  * @retval None
  */
void ESP8266_USART2_SendByte(uint8_t Byte)
{
	USART_SendData(USART2,Byte);																																			//�������������Byte��д��TDR�Ĵ�����
																																																		//д��֮����Ҫ�ȴ�һ�£���TDR������ת�Ƶ���λ�Ĵ����Ϳ����ˣ�������ݻ���TDR�Ĵ����У���д�����ݾͻ�������ݸ��ǣ������ڷ���֮����Ҫ�ȴ�һ�±�־λ
	while(USART_GetFlagStatus(USART2,USART_FLAG_TXE) == RESET);																				//�������ݼĴ����ձ�־λ���ȴ�TXE��1��������Ҫ��һ��whileѭ�� TXE:�������ݼĴ����գ��������־λ�Զ���0�������ֶ���λ
}

/*�����ַ�������*/
void ESP8266_USART2_SendString(char *String)																												//��uint8_t *Ҳ���ԣ������ַ����Դ�һ����־λ�����ԾͲ���Ҫ�ٴ��ݳ��Ȳ�����
{
	for(uint8_t i = 0;String[i] != '\0';i++)																													//ѭ�������Ϳ����ñ�־λ���ж���,��'\0'�ǿ��ַ���ת���ַ���ʾ��ʽ����ֱ��д0��һ����
	{
		ESP8266_USART2_SendByte(String[i]);																															//��String�ַ���һ����ȡ������ͨ��SendByte����
	}
}

/**
  * @brief  printf��װ����					
  * @note   ��һ�������ǽ����ַ������ڶ����ǽ��տɱ�����б�
  * @param  *format,...
  * @retval None
  */
void ESP8266_USART2_Printf(char *format, ...)																													
{
	char String[100];
	va_list arg;																																											//arg�Ƕ���һ�������б����
	va_start(arg,format);																																							//��formatλ�ÿ�ʼ���ղ���������arg����
	vsprintf(String,format,arg);																																			//��װ��ʽҪ��vsprintf����Ϊsprintfֻ�ܽ���ֱ��д�Ĳ�������ӡ�ַ�����ʽ��format����������arg��
	va_end(arg);																																											//�ͷŲ�����
	ESP8266_USART2_SendString(String);
}

//���������ķ�װ
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
  * @brief  ESP8266���ӳ�ʼ������					
  * @note   ���º�����Ҫ���ڽ�ESP8266ģ������ΪAPģʽ�����ӵ�Wi-Fi�ȵ㡣
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
  * @brief  ESP8266����MQTTģʽ����					
  * @note   ���º�����Ҫ���ڽ�ESP8266ģ������MQTTģʽ���������Ӧ��MQTT��������
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
//  * @brief  USART2�жϴ�����
//  *	@brief	����16�����жϷ���ATָ���ESP8266�Ļش��ַ��������ڽ���ESP8266�Ƿ�ش���OK��ָ�����ش���ִ�ж�Ӧ��ָ�
//  * @brief	״̬����һ����Ϊ3�����ֱ���0��1��2��Ҳ���ǵȴ���ͷ���������ݺ͵ȴ���β
//  * @param  ��
//  * @retval ��
//  */
//void USART2_IRQHandler(void)
//{
//	static uint8_t RxState = 0;//״̬����S=0
//	static uint8_t pRxPacket = 0;//ָʾ���յ���һ������
//	
//	if(USART_GetITStatus(USART2,USART_IT_RXNE) == SET)//���RXNEȷʵ��1�ˣ��ͽ�if
//	{
//		uint8_t RxData = USART_ReceiveData(USART2);//��ȡ���յ�������
//		switch (RxState)
//		{
//			case 0:
//			{
//				if (RxData == 'O')
//				{
//					RxState = 1;
//					pRxPacket = 0;//���㣬Ϊ��һ�ν�����׼��
//				}
//			}
//			case 1:
//			{
//				if(RxData == '\r')//���ж��ǲ��ǰ�β
//				{
//					RxState = 2;
//				}
//				else
//				{	
//					esp8266RxPacket[pRxPacket++] = RxData;//ÿ��һ�ν���״̬�����ݾ�ת��һ�λ������飬ͬʱ���λ��++
//				}
//			}
//			case 2:
//			{
//				if (RxData == '\n')//�ȴ��ڶ�����β
//				{
//					RxState = 0;//�ص������״̬
//					esp8266RxPacket[pRxPacket] = '\0';//��һ���ַ���������־λ\0����ȷ���ַ����ĳ���
//					esp8266RxFlag = 1;//�����������ݰ��Ѿ��յ��ˣ���һ����־λ
//				}
//			}
//		}
//		USART_ClearITPendingBit(USART2,USART_IT_RXNE);//if�Ƿ�Ҫ�����־λ�أ������ȡ��DR���ͻ��Զ���������û��ȡ����Ҫ�ֶ����
//	}
//}	
