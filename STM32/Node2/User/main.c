#include "stm32f10x.h"                  // Device header
#include "Systick.h"
#include "OLED.h"
#include "LoRa.h"
#include "MQ2.h"
#include "ADCx.h"
#include "Beep.h"
#include "IR.h"
#include "Servo.h"

uint16_t fireValue;
uint8_t buff[30];//参数显示缓存数组
float ppmValue;
int main(void){
	SysTick_Init(72);
	LoRa_USART3_Trans_Mode_Init(9600);
	MQ2_Init();
	Beep_Init();
	IR_Init();
	Servo_Init();
	while(1)
	{
//		LoRa_USART3_Printf("%.02fppm\r\n",ppmValue);
		ppmValue = MQ2_GetData_PPM();
		fireValue = IR_FireData();
		switch (fireValue)
		{
			case 0:
			{
				if(ppmValue >= 10)
				{
					Beep_ON();
					Servo_SetAngle(180);
					LoRa_USART3_IdentifierPkt();
					LoRa_USART3_SendArray(loRaSensorMQ2Identifier,1);
					LoRa_USART3_SendArray(loRaExecutorStatusOn,1);
					LoRa_USART3_SendArray(loRaSensorFireIdentifier,1);
					LoRa_USART3_SendArray(loRaExecutorStatusOff,1);
					LoRa_USART3_SendArray(loRaExecutorBuzzer,1);
					LoRa_USART3_SendArray(loRaExecutorStatusOn,1);
					LoRa_USART3_SendArray(loRaExecutorServo,1);
					LoRa_USART3_SendArray(loRaExecutorStatusOn,1);
					break;
				}
				else
				{
					Beep_OFF();
					Servo_SetAngle(0);
					LoRa_USART3_IdentifierPkt();
					LoRa_USART3_SendArray(loRaSensorMQ2Identifier,1);
					LoRa_USART3_SendArray(loRaExecutorStatusOff,1);
					LoRa_USART3_SendArray(loRaSensorFireIdentifier,1);
					LoRa_USART3_SendArray(loRaExecutorStatusOff,1);
					LoRa_USART3_SendArray(loRaExecutorBuzzer,1);
					LoRa_USART3_SendArray(loRaExecutorStatusOff,1);
					LoRa_USART3_SendArray(loRaExecutorServo,1);
					LoRa_USART3_SendArray(loRaExecutorStatusOff,1);
					break;
				}
			}
			case 1:
			{
				if(ppmValue >= 10)
				{
					Beep_ON();
					Servo_SetAngle(180);
					LoRa_USART3_IdentifierPkt();
					LoRa_USART3_SendArray(loRaSensorMQ2Identifier,1);
					LoRa_USART3_SendArray(loRaExecutorStatusOn,1);
					LoRa_USART3_SendArray(loRaSensorFireIdentifier,1);
					LoRa_USART3_SendArray(loRaExecutorStatusOn,1);
					LoRa_USART3_SendArray(loRaExecutorBuzzer,1);
					LoRa_USART3_SendArray(loRaExecutorStatusOn,1);
					LoRa_USART3_SendArray(loRaExecutorServo,1);
					LoRa_USART3_SendArray(loRaExecutorStatusOn,1);
					break;
				}
				else
				{
					Beep_ON();
					Servo_SetAngle(180);
					LoRa_USART3_IdentifierPkt();
					LoRa_USART3_SendArray(loRaSensorMQ2Identifier,1);
					LoRa_USART3_SendArray(loRaExecutorStatusOff,1);
					LoRa_USART3_SendArray(loRaSensorFireIdentifier,1);
					LoRa_USART3_SendArray(loRaExecutorStatusOn,1);
					LoRa_USART3_SendArray(loRaExecutorBuzzer,1);
					LoRa_USART3_SendArray(loRaExecutorStatusOn,1);
					LoRa_USART3_SendArray(loRaExecutorServo,1);
					LoRa_USART3_SendArray(loRaExecutorStatusOn,1);
					break;
				}
			}
		}
		delay_ms(1500);
	}
}
