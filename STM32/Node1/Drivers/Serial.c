#include "Serial.h"

/*USART1������ֻ֧�ִ��ݼ��ֽڵ����ݣ��紫�ݸ���������Ҫ�õ����ݰ�*/
/*���뺺�ַ�ֹ����ķ�����
1��ȷ�ϳ������ʹ������ֱ����ʽһ�������綼��UTF-8
2����Ҫ��C/C++�������м��������Ĳ�����--no-multibyte-chars
*/

uint8_t Serial_RXData;
uint8_t Serial_RXFlag;

uint8_t Serial_TxPacket[9];//ֻ�洢���ͻ���յ��غ�����
uint8_t Serial_RxPacket[4];

void Serial_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;//���ڷ��ͣ�PA9
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//���ڽ��գ�PA10
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;//9600�����ʣ�д���USART_Init�����ڲ����Զ����9600��Ӧ�ķ�Ƶϵ������д��BRR�Ĵ���
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//Ӳ��������,��ʹ������
	USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;//����ģʽ�������ʹ����������ģʽ���û���ţ�����ģʽ�ͽ���ģʽ
	USART_InitStructure.USART_Parity = USART_Parity_No;//��У��λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һλֹͣλ
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�������ҪУ�飬�ֳ���ѡ��8λ
	USART_Init(USART1,&USART_InitStructure);
	//�����Ǵ��ڵĲ�ѯģʽ�����ʹ���жϣ�����Ҫ�����жϣ�����NVIC
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);//ѡ��RXNE���ж�,������RXNE��־λ��NVIC����������RXNE��־λ��1���ͻ���NVIC�����жϣ�֮��������жϺ����н�������
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����
	NVIC_InitTypeDef NVIC_InitStructure;//��ʼ��NVIC��USART1ͨ��
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);//ָ��NVIC_InitStructure�ĵ�ַ
	USART_Cmd(USART1,ENABLE);
	
}

/*���͵����ֽں�������������������Ϳ��Դ�TX���ŷ���һ���ֽ�����*/
void Serial_SendByte(uint8_t Byte)
{
	USART_SendData(USART1,Byte);//�������������Byte��д��TDR�Ĵ�����
	//д��֮����Ҫ�ȴ�һ�£���TDR������ת�Ƶ���λ�Ĵ����Ϳ����ˣ�������ݻ���TDR�Ĵ����У���д�����ݾͻ�������ݸ��ǣ������ڷ���֮����Ҫ�ȴ�һ�±�־λ
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);//�������ݼĴ����ձ�־λ���ȴ�TXE��1��������Ҫ��һ��whileѭ�� TXE:�������ݼĴ����գ��������־λ�Զ���0�������ֶ���λ
}

/*�������麯����һ������hexģʽ��*/
void Serial_SendArray(uint8_t *Array,uint16_t Length)//uint8_t��ָ�����ͣ�ָ�������������׵�ַ,���������޷��ж��Ƿ������������Ҫ�ٴ���һ��Length����
{
	for(uint16_t i = 0;i < Length;i ++)//forѭ��ִ��Length�Σ����Զ�Array���ݽ��б�����ʵ�ʶ������鲻Ҫ����uint16_t�ķ�Χ����
	{
		Serial_SendByte(Array[i]);//����ȡ������Array��ÿһ��
	}
}

/*�����ַ�������*/
void Serial_SendString(char *String)//��uint8_t *Ҳ���ԣ������ַ����Դ�һ����־λ�����ԾͲ���Ҫ�ٴ��ݳ��Ȳ�����
{
	for(uint8_t i = 0;String[i] != '\0';i++)//ѭ�������Ϳ����ñ�־λ���ж���,��'\0'�ǿ��ַ���ת���ַ���ʾ��ʽ����ֱ��д0��һ����
	{
		Serial_SendByte(String[i]);//��String�ַ���һ����ȡ������ͨ��SendByte����
	}
}

/*printf�����ض���*/
int fputc(int ch,FILE *f)//fputc��printf�����ĵײ㣬printf�����ڴ�ӡ��ʱ�򣬾��ǲ��ϵ���fputc������ӡ
{
	Serial_SendByte(ch);
	return ch;
}

/*printf������װ*/
void Serial_Printf(char *format, ...)//��һ���ǽ����ַ������ڶ����ǽ��տɱ�����б�
{
	char String[100];
	va_list arg;//����һ�������б����
	va_start(arg,format);//��formatλ�ÿ�ʼ���ղ���������arg����
	vsprintf(String,format,arg);//��װ��ʽҪ��vsprintf����Ϊsprintfֻ�ܽ���ֱ��д�Ĳ�������ӡ�ַ�����ʽ��format����������arg��
	va_end(arg);//�ͷŲ�����
	Serial_SendString(String);
}

/*����hex���ݰ���hex���ݰ��ĸ�ʽΪ�̶�����������ͷ��β�����а�ͷΪFF���غ����ݹ̶�Ϊ4�ֽڣ���βΪFE*/
void Serial_SendPacket(void)
{
	Serial_SendArray(Serial_TxPacket, 9);//�����غ�
}

/*����hex���ݰ�*/

/*���������ķ�װ*/
uint8_t Serial_GetRxFlag(void)
{
	if(Serial_RXFlag == 1)
	{
		Serial_RXFlag = 0;
		return 1;
	}
	return 0;
}

void USART1_IRQHandler(void)
{
	//��״̬�������߼�
	//״̬����һ����Ϊ3�����ֱ���0��1��2��Ҳ���ǵȴ���ͷ���������ݺ͵ȴ���β
	static uint8_t RxState = 0;//״̬����S=0
	static uint8_t pRxPacket = 0;//ָʾ���յ���һ������
	
	if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET)//���RXNEȷʵ��1�ˣ��ͽ�if
	{
		//�����ֽڣ��ȶ�ȡ��ģ��ı�����
		//ע�⣺���ʹ��3��if�����ܻᵼ������������3������ͬʱ����������
		
		uint8_t RxData = USART_ReceiveData(USART1);//��ȡRxData
		if (RxState == 0)
		{
			if (RxData == 0xFF)
			{
				RxState = 1;
				pRxPacket = 0;//���㣬Ϊ��һ�ν�����׼��
			}
		}
		else if (RxState == 1)
		{
			Serial_RxPacket[pRxPacket] = RxData;//ÿ��һ�ν���״̬�����ݾ�ת��һ�λ������飬ͬʱ���λ��++
			pRxPacket++;//�ƶ�����һ��λ��
			if(pRxPacket >= 4)
			{
				RxState = 2;
			}
		}
		else if (RxState == 2)
		{
			if (RxData == 0xFE)
			{
				RxState = 0;//�ص������״̬
				Serial_RXFlag = 1;//�����������ݰ��Ѿ��յ��ˣ���һ����־λ
			}
		}
		
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);//if�Ƿ�Ҫ�����־λ�أ������ȡ��DR���ͻ��Զ���������û��ȡ����Ҫ�ֶ����
	}
}
