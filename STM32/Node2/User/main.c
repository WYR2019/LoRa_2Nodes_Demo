#include "stm32f10x.h"                  // Device header
#include "Systick.h"
#include "OLED.h"
#include "LoRa.h"
#include "MQ2.h"
#include "ADCx.h"
#include "Beep.h"
#include "IR.h"
#include "Servo.h"

typedef struct {
  uint16_t usFireValue;
  float fPpmValue;
} FireSensor_t;

int main(void){
  FireSensor_t xFireSensor;
  SysTick_Init(72);
  vLoRaUSART3EnableInit(115200);
  MQ2_Init();
  Beep_Init();
  IR_Init();
  Servo_Init();
  while(1)
  {
    xFireSensor.fPpmValue = MQ2_GetData_PPM();
    xFireSensor.usFireValue = IR_FireData();
    switch (xFireSensor.usFireValue)
    {
      case 0:
      {
        if(xFireSensor.fPpmValue >= 10)
        {
          Beep_ON();
          Servo_SetAngle(180);
          vLoRaUSART3GateIdPkt();
          vLoRaUSART3SendArray(&xLoRaSensorID.ucMQ2,1);
          vLoRaUSART3SendArray(&xLoRaExecutorStatus.ucStatusOn,1);
          vLoRaUSART3SendArray(&xLoRaSensorID.ucFire,1);
          vLoRaUSART3SendArray(&xLoRaExecutorStatus.ucStatusOff,1);
          vLoRaUSART3SendArray(&xLoRaExecutorID.ucBuzzer,1);
          vLoRaUSART3SendArray(&xLoRaExecutorStatus.ucStatusOn,1);
          vLoRaUSART3SendArray(&xLoRaExecutorID.ucServo,1);
          vLoRaUSART3SendArray(&xLoRaExecutorStatus.ucStatusOn,1);
          break;
        }
        else
        {
          Beep_OFF();
          Servo_SetAngle(0);
          vLoRaUSART3GateIdPkt();
          vLoRaUSART3SendArray(&xLoRaSensorID.ucMQ2,1);
          vLoRaUSART3SendArray(&xLoRaExecutorStatus.ucStatusOff,1);
          vLoRaUSART3SendArray(&xLoRaSensorID.ucFire,1);
          vLoRaUSART3SendArray(&xLoRaExecutorStatus.ucStatusOff,1);
          vLoRaUSART3SendArray(&xLoRaExecutorID.ucBuzzer,1);
          vLoRaUSART3SendArray(&xLoRaExecutorStatus.ucStatusOff,1);
          vLoRaUSART3SendArray(&xLoRaExecutorID.ucServo,1);
          vLoRaUSART3SendArray(&xLoRaExecutorStatus.ucStatusOff,1);
          break;
        }
      }
      case 1:
      {
        if(xFireSensor.fPpmValue >= 10)
        {
          Beep_ON();
          Servo_SetAngle(180);
          vLoRaUSART3GateIdPkt();
          vLoRaUSART3SendArray(&xLoRaSensorID.ucMQ2,1);
          vLoRaUSART3SendArray(&xLoRaExecutorStatus.ucStatusOn,1);
          vLoRaUSART3SendArray(&xLoRaSensorID.ucFire,1);
          vLoRaUSART3SendArray(&xLoRaExecutorStatus.ucStatusOn,1);
          vLoRaUSART3SendArray(&xLoRaExecutorID.ucBuzzer,1);
          vLoRaUSART3SendArray(&xLoRaExecutorStatus.ucStatusOn,1);
          vLoRaUSART3SendArray(&xLoRaExecutorID.ucServo,1);
          vLoRaUSART3SendArray(&xLoRaExecutorStatus.ucStatusOn,1);
          break;
        }
        else
        {
          Beep_ON();
          Servo_SetAngle(180);
          vLoRaUSART3GateIdPkt();
          vLoRaUSART3SendArray(&xLoRaSensorID.ucMQ2,1);
          vLoRaUSART3SendArray(&xLoRaExecutorStatus.ucStatusOff,1);
          vLoRaUSART3SendArray(&xLoRaSensorID.ucFire,1);
          vLoRaUSART3SendArray(&xLoRaExecutorStatus.ucStatusOn,1);
          vLoRaUSART3SendArray(&xLoRaExecutorID.ucBuzzer,1);
          vLoRaUSART3SendArray(&xLoRaExecutorStatus.ucStatusOn,1);
          vLoRaUSART3SendArray(&xLoRaExecutorID.ucServo,1);
          vLoRaUSART3SendArray(&xLoRaExecutorStatus.ucStatusOn,1);
          break;
        }
      }
    }
    delay_ms(1500);
  }
}
