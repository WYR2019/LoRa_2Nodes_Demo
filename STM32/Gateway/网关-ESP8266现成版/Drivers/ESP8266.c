#include "ESP8266.h"		 

uint16_t usart2ReceiveFlag = 0;                                  									//接收状态	

uint8_t ESP8266_RX_BUF[ESP8266_USART2_MAX_RECV_LEN]; 															//ESP8266所在的接收缓冲区，存储的是8266的最大接收字节1024字节
uint8_t ESP8266_TX_BUF[ESP8266_USART2_MAX_SEND_LEN]; 															//ESP8266所在的发送缓冲区，存储的是8266的最大发送字节1024字节
uint8_t USARTx_RX_BUF[USART2_RX_MAX_SIZE];     																		//接收缓冲,最大USART_REC_LEN个字节
uint8_t pubPayloadBuffer[BUFFER_SIZE];
int pubLedSwitch, pubFanSwitch, pubHumidifierSwitch;
int sendData,size = 0;
extern uint8_t USART1_RX_BUF[USART2_RX_MAX_SIZE];

void ESP8266_USART2_Init()
{
  // GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure; 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

  /* 配置GPIO的模式和IO口 */
  GPIO_InitStructure.GPIO_Pin = ESP8266_GPIO_PIN_TX; 															// TX 串口输出PA2
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;     														// 复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);              														// 初始化串口输入IO
  GPIO_InitStructure.GPIO_Pin = ESP8266_GPIO_PIN_RX;          										// RX 串口输入PA3
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 													// 模拟输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);              														// 初始化GPIO

  // USART2 初始化设置
  USART_InitTypeDef USART_InitStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); 													// 更改为串口2时钟
  USART_InitStructure.USART_BaudRate = 115200;                                    //115200波特率，写完后，USART_Init函数内部会自动算好115200对应的分频系数，并写到BRR寄存器
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     // 字长为8位数据格式
  USART_InitStructure.USART_StopBits = USART_StopBits_1;                					// 一个停止位
  USART_InitStructure.USART_Parity = USART_Parity_No;                   					// 无奇偶校验位
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //硬件数据流控制,不使用流控
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;       					// 串口模式，如果既使用输入和输出模式就用或符号
  USART_Init(USART2, &USART_InitStructure);                             					// 初始化串口2
  USART_Cmd(USART2, ENABLE);  																										// 使能串口2

  USART_ClearFlag(USART2, USART_FLAG_TC);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // 开启相关中断

  // Usart2 NVIC 配置
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;               								// 串口2中断通道
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;       								// 抢占优先级3
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;               								// 子优先级3
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                 								// IRQ通道使能
  NVIC_Init(&NVIC_InitStructure);                                  								// 根据指定的参数初始化VIC寄存器
}

void ESP8266_Printf(char* format,...)																							//format是格式化字符串（像：”%3d%6.2f%#x%o”,%与#合用时，自动在十六进制数前面加上0x）
{
	uint16_t i,j; 
	va_list arg; 																																		//存储可变参数列表
	va_start(arg,format); 																													//初始化arg变量，以便它指向第一个可选参数。format是可变参数列表中最后一个固定参数。
	vsprintf((char*)ESP8266_TX_BUF,format,arg); 																		//将格式化的字符串输出到ESP8266_TX_BUF缓冲区。
	va_end(arg); 																																		//清理arg防止内存泄漏
	i=strlen((const char*)ESP8266_TX_BUF); 																					//获取字符串长度
	for(j=0;j<i;j++) 																																//确定发送的字符串数量
	{
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET); 										  //等待发送完成，TC时transmission complete的缩写，整段函数的意思是检查USART2的发送完成标志
		USART_SendData(USART2,ESP8266_TX_BUF[j]); 																		//将存储的字符串数组发出去
	}
}

void ESP8266_Connection(void)
{
	/****初始化esp8266******/
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
  * @brief  USART2中断函数，用于接收并处理USART2发过来的数据。
	* @brief	如果接收到换行符，则将状态标志位置1，并执行相应函数；反之
  * @param  无
  * @retval 无
  */
void USART2_IRQHandler(void)
{  
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断
	{  
		uint32_t rxData = USART_ReceiveData(USART2); //读取接收到的数据
		if ((usart2ReceiveFlag & 0x8000) == 0) //检查 ESP8266_RX_STA 的高位（第15位）是否为 0，以确定当前接收是否未完成；&为“与”运算，当两位都为0时，执行相应的函数。
		{
			if (usart2ReceiveFlag & 0x4000) //检查 ESP8266_RX_STA 的第14位（0x4000）是否为 1，以确定是否已经接收到回车字符 (0x0D)
			{
				if (rxData != 0x0A) 
					usart2ReceiveFlag = 0;//如果已经接收到 0x0D（回车），但接收到的数据不是 0x0A（换行），则认为接收错误，将接收状态重置为 0。
				else
				{  
					usart2ReceiveFlag |= 0x8000; //如果接收到的数据是 0x0A，说明数据接收完成。
					ESP8266_Subscribe_Topics_Actions(); //将 ESP8266_RX_STA 的高位设置为 1，表示接收完成，并调用 SubscribeActions() 函数处理接收到的数据。
					usart2ReceiveFlag = 0; //最后，将接收状态重置为 0。
				}
			}
			else //还没收到0X0D
			{   
				if (rxData == 0x0D) 
				usart2ReceiveFlag |= 0x4000; //如果接收到的数据是 0x0D，将 ESP8266_RX_STA 的第14位设置为 1，表示已经接收到回车。
				else
				{
					USARTx_RX_BUF[usart2ReceiveFlag & 0x3FFF] = rxData; //如果接收到的数据不是 0x0D，则将其存储到接收缓冲区 USARTx_RX_BUF 中，
					usart2ReceiveFlag++; //并更新接收状态计数器（ESP8266_RX_STA）。
					if (usart2ReceiveFlag > (USART2_RX_MAX_SIZE - 1)) usart2ReceiveFlag = 0; //检查 ESP8266_RX_STA 是否超过了缓冲区的最大长度 USART_REC_LEN - 1。如果超过，意味着接收数据出错，将接收状态重置为 0。  
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
		size = strlen((char*)pubPayloadBuffer); // 获取字符占用的大小
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
