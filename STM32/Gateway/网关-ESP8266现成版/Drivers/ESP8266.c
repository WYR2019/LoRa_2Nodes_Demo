#include "ESP8266.h"		 

uint16_t usart2ReceiveFlag = 0;                                  									//����״̬	

uint8_t ESP8266_RX_BUF[ESP8266_USART2_MAX_RECV_LEN]; 															//ESP8266���ڵĽ��ջ��������洢����8266���������ֽ�1024�ֽ�
uint8_t ESP8266_TX_BUF[ESP8266_USART2_MAX_SEND_LEN]; 															//ESP8266���ڵķ��ͻ��������洢����8266��������ֽ�1024�ֽ�
uint8_t USARTx_RX_BUF[USART2_RX_MAX_SIZE];     																		//���ջ���,���USART_REC_LEN���ֽ�
uint8_t pubPayloadBuffer[BUFFER_SIZE];
int pubLedSwitch, pubFanSwitch, pubHumidifierSwitch;
int sendData,size = 0;
extern uint8_t USART1_RX_BUF[USART2_RX_MAX_SIZE];

void ESP8266_USART2_Init()
{
  // GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure; 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

  /* ����GPIO��ģʽ��IO�� */
  GPIO_InitStructure.GPIO_Pin = ESP8266_GPIO_PIN_TX; 															// TX �������PA2
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;     														// �����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);              														// ��ʼ����������IO
  GPIO_InitStructure.GPIO_Pin = ESP8266_GPIO_PIN_RX;          										// RX ��������PA3
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 													// ģ������
  GPIO_Init(GPIOA, &GPIO_InitStructure);              														// ��ʼ��GPIO

  // USART2 ��ʼ������
  USART_InitTypeDef USART_InitStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); 													// ����Ϊ����2ʱ��
  USART_InitStructure.USART_BaudRate = 115200;                                    //115200�����ʣ�д���USART_Init�����ڲ����Զ����115200��Ӧ�ķ�Ƶϵ������д��BRR�Ĵ���
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     // �ֳ�Ϊ8λ���ݸ�ʽ
  USART_InitStructure.USART_StopBits = USART_StopBits_1;                					// һ��ֹͣλ
  USART_InitStructure.USART_Parity = USART_Parity_No;                   					// ����żУ��λ
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //Ӳ������������,��ʹ������
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;       					// ����ģʽ�������ʹ����������ģʽ���û����
  USART_Init(USART2, &USART_InitStructure);                             					// ��ʼ������2
  USART_Cmd(USART2, ENABLE);  																										// ʹ�ܴ���2

  USART_ClearFlag(USART2, USART_FLAG_TC);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // ��������ж�

  // Usart2 NVIC ����
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;               								// ����2�ж�ͨ��
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;       								// ��ռ���ȼ�3
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;               								// �����ȼ�3
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                 								// IRQͨ��ʹ��
  NVIC_Init(&NVIC_InitStructure);                                  								// ����ָ���Ĳ�����ʼ��VIC�Ĵ���
}

void ESP8266_Printf(char* format,...)																							//format�Ǹ�ʽ���ַ������񣺡�%3d%6.2f%#x%o��,%��#����ʱ���Զ���ʮ��������ǰ�����0x��
{
	uint16_t i,j; 
	va_list arg; 																																		//�洢�ɱ�����б�
	va_start(arg,format); 																													//��ʼ��arg�������Ա���ָ���һ����ѡ������format�ǿɱ�����б������һ���̶�������
	vsprintf((char*)ESP8266_TX_BUF,format,arg); 																		//����ʽ�����ַ��������ESP8266_TX_BUF��������
	va_end(arg); 																																		//����arg��ֹ�ڴ�й©
	i=strlen((const char*)ESP8266_TX_BUF); 																					//��ȡ�ַ�������
	for(j=0;j<i;j++) 																																//ȷ�����͵��ַ�������
	{
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET); 										  //�ȴ�������ɣ�TCʱtransmission complete����д�����κ�������˼�Ǽ��USART2�ķ�����ɱ�־
		USART_SendData(USART2,ESP8266_TX_BUF[j]); 																		//���洢���ַ������鷢��ȥ
	}
}

void ESP8266_Connection(void)
{
	/****��ʼ��esp8266******/
	ESP8266_Printf("AT+RESTORE\r\n");
	delay_ms(5000);
	ESP8266_Printf("AT+CWMODE=1\r\n");
	delay_ms(5000);
	ESP8266_Printf("AT+CWJAP=\"%s\",\"%s\"\r\n",WIFINAME,WIFIPASSWORD);
	delay_ms(5000);
	ESP8266_Printf("AT+MQTTUSERCFG=0,1,\"NULL\",\"%s\",\"%s\",0,0,\"\"\r\n",MQTTUSERNAME,MQTTPASSWORD);
	delay_ms(5000);
	ESP8266_Printf("AT+MQTTCLIENTID=0,\"%s\"\r\n",MQTTCLIENTID);
	delay_ms(5000);
	ESP8266_Printf("AT+MQTTCONN=0,\"%s\",1883,1\r\n",MQTTBROKERIP);
	delay_ms(5000);
	ESP8266_Printf("AT+MQTTSUB=0,\"%s\",1\r\n",SUBSCRIBETOPIC);
	delay_ms(5000);
}

/**
  * @brief  USART2�жϺ��������ڽ��ղ�����USART2�����������ݡ�
	* @brief	������յ����з�����״̬��־λ��1����ִ����Ӧ��������֮
  * @param  ��
  * @retval ��
  */
void USART2_IRQHandler(void)
{  
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�
	{  
		uint32_t rxData = USART_ReceiveData(USART2); //��ȡ���յ�������
		if ((usart2ReceiveFlag & 0x8000) == 0) //��� ESP8266_RX_STA �ĸ�λ����15λ���Ƿ�Ϊ 0����ȷ����ǰ�����Ƿ�δ��ɣ�&Ϊ���롱���㣬����λ��Ϊ0ʱ��ִ����Ӧ�ĺ�����
		{
			if (usart2ReceiveFlag & 0x4000) //��� ESP8266_RX_STA �ĵ�14λ��0x4000���Ƿ�Ϊ 1����ȷ���Ƿ��Ѿ����յ��س��ַ� (0x0D)
			{
				if (rxData != 0x0A) 
					usart2ReceiveFlag = 0;//����Ѿ����յ� 0x0D���س����������յ������ݲ��� 0x0A�����У�������Ϊ���մ��󣬽�����״̬����Ϊ 0��
				else
				{  
					usart2ReceiveFlag |= 0x8000; //������յ��������� 0x0A��˵�����ݽ�����ɡ�
					ESP8266_Subscribe_Topics_Actions(); //�� ESP8266_RX_STA �ĸ�λ����Ϊ 1����ʾ������ɣ������� SubscribeActions() ����������յ������ݡ�
					usart2ReceiveFlag = 0; //��󣬽�����״̬����Ϊ 0��
				}
			}
			else //��û�յ�0X0D
			{   
				if (rxData == 0x0D) 
				usart2ReceiveFlag |= 0x4000; //������յ��������� 0x0D���� ESP8266_RX_STA �ĵ�14λ����Ϊ 1����ʾ�Ѿ����յ��س���
				else
				{
					USARTx_RX_BUF[usart2ReceiveFlag & 0x3FFF] = rxData; //������յ������ݲ��� 0x0D������洢�����ջ����� USARTx_RX_BUF �У�
					usart2ReceiveFlag++; //�����½���״̬��������ESP8266_RX_STA����
					if (usart2ReceiveFlag > (USART2_RX_MAX_SIZE - 1)) usart2ReceiveFlag = 0; //��� ESP8266_RX_STA �Ƿ񳬹��˻���������󳤶� USART_REC_LEN - 1�������������ζ�Ž������ݳ���������״̬����Ϊ 0��  
				}         
			}
		}          
	}
}

void ESP8266_USART1_Scan_Actions(void)
{
	sscanf((char *)USART1_RX_BUF,"{%d,%d,%d,%d,%d,%d,%d,%d,}",&jsonTemp, &jsonHumi, &jsonFan, &jsonHumidifier, &jsonSmoke, &jsonFire, &jsonBeep, &jsonServo);
	memset(USART1_RX_BUF,0,600);
}

void ESP8266_MQTT_Publish(void)
{
	if(sendData >= 30)
	{
		sendData = 0;	
		sprintf((char*)pubPayloadBuffer,"{\"%s\":%d,\"%s\":%d,\"%s\":%d,\"%s\":%d,\"%s\":%d,\"%s\":%d,\"%s\":%d,\"%s\":%d}",ID_TEMP,jsonTemp,ID_HUMI,jsonHumi,ID_FAN,jsonFan,ID_HUMIDIFIER,jsonHumidifier,ID_SMOKE,jsonSmoke,ID_FIRE,jsonFire,ID_BEEP,jsonBeep,ID_SERVO,jsonServo);	
		size = strlen((char*)pubPayloadBuffer); // ��ȡ�ַ�ռ�õĴ�С
		delay_ms(1000);
		ESP8266_Printf("AT+MQTTPUBRAW=0,\"%s\",%d,1,0\r\n",PUBLISHTOPIC,size);
		delay_ms(1000);
		ESP8266_Printf("%s",pubPayloadBuffer);
	}
}

void ESP8266_Subscribe_Topics_Actions(void)
{
	if(strstr((const char*)USARTx_RX_BUF,"@LED_ON"))
	{
		pubLedSwitch = 1;
	}
	if(strstr((const char*)USARTx_RX_BUF,"@LED_OFF"))
	{
		pubLedSwitch = 0;
	}
	if(strstr((const char*)USARTx_RX_BUF,"@Fan_ON"))
	{
		pubFanSwitch = 1;
	}
	if(strstr((const char*)USARTx_RX_BUF,"@Fan_OFF"))
	{
		pubFanSwitch = 0;
	}
	if(strstr((const char*)USARTx_RX_BUF,"@Humidifier_ON"))
	{
		pubHumidifierSwitch = 1;
	}
	if(strstr((const char*)USARTx_RX_BUF,"@Humidifier_OFF"))
	{
		pubHumidifierSwitch = 0;
	}
	memset(USARTx_RX_BUF,0,600);		
}

void ESP8266_Publish_Topics_Actions(void)
{
	if(pubFanSwitch == 1)
	{
		jsonFan = 1;
	}
	else
	{
		jsonFan = 0;
	}
	if(pubHumidifierSwitch == 1)
	{
		jsonHumidifier = 1;
	}
	else
	{
		jsonHumidifier = 0;
	}
}
