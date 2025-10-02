#include "SysTick.h"

//��ʼ���ӳٺ���
//SYSTICK��ʱ�ӹ̶�ΪAHBʱ�ӵ�1/8
//SYSCLK:ϵͳʱ��Ƶ��
void SysTick_Init(uint8_t ucSYSCLK)
{
  SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); 
  xUcFacInit.ucFacUs=ucSYSCLK/8;					
  xUcFacInit.usFacMs=(uint16_t)xUcFacInit.ucFacUs*1000;				   
}								    

//��ʱnus
//nusΪҪ��ʱ��us��.		    								   
void delay_us(uint32_t ulNus)
{
  uint32_t ulTemp;	    	 
  SysTick->LOAD=ulNus*xUcFacInit.ucFacUs; 					      //ʱ�����	  		 
  SysTick->VAL=0x00;        					                    //��ռ�����
  SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;                //��ʼ����	  
  do
  {
    ulTemp=SysTick->CTRL;
  }while((ulTemp&0x01)&&!(ulTemp&(1<<16)));		            //�ȴ�ʱ�䵽��   
  SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	              //�رռ�����
  SysTick->VAL =0X00;      					                      //��ռ�����	 	
}

//��ʱnms
//ע��nms�ķ�Χ
//SysTick->LOADΪ24λ�Ĵ���,����,�����ʱΪ:
//nms<=0xffffff*8*1000/SYSCLK
//SYSCLK��λΪHz,nms��λΪms
//��72M������,nms<=1864

/**
  * @brief  ���뼶��ʱ����
  * @note   ����Systick��ʱ��Ϊ24λ��ʱ������װ�ص����ֵΪ0xFFFFFF��Ҳ����16777215��������ص�ֵ����0xFFFFFF����ʵ�ʼ��ص�ֵ��ȡģ���ֵ��������ʱʱ�����̡�
  *         ���Գ���1.864s����ʱ�ᵼ�¼�����������Ӷ����¶�ʱΪxms-16777215�Ľ����
  * @note   ���º������ж���ʱ�Ƿ񳬹������ʱ����������ֳɶ��1000ms����
  * @param  None
  * @retval None
  */
void delay_ms(uint16_t usNms)
{
  uint32_t ulTemp;
  while (usNms)
  {
    uint16_t current = (usNms > 1000) ? 1000 : usNms;
    usNms -= current;

    SysTick->LOAD = (uint32_t)current * xUcFacInit.usFacMs;
    SysTick->VAL = 0x00;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    do
    {
      ulTemp = SysTick->CTRL;
    } while ((ulTemp & 0x01) && !(ulTemp & (1 << 16)));
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->VAL = 0X00;
  }   
}
