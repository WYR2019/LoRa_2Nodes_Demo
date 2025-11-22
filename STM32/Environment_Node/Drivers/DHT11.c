#include "DHT11.h"
			
//复位DHT11
void vDht11Rst(void)	   
{                 
  vDht11Mode(OUT); 	//SET OUTPUT
  DHT11_Low; 	      //拉低DQ
  vDelayMs(20);    	//主机拉低18~30ms
  DHT11_High; 			//DQ=1 
  vDelayUs(13);     	//主机拉高10~35us
}

//等待DHT11的回应
//返回1:未检测到DHT11的存在
//返回0:存在
uint8_t vDht11Check(void) 	   
{   
  uint8_t retry=0;
  vDht11Mode(IN);//SET INPUT	 
  while (GPIO_ReadInputDataBit(DHT11_GPIO_PORT,DHT11_GPIO_PIN)&&retry<100)//DHT11会拉低40~80us
  {
    retry++;
    vDelayUs(1);
  };	 
  if(retry>=100)return 1;
  else retry=0;
    while (!GPIO_ReadInputDataBit(DHT11_GPIO_PORT,DHT11_GPIO_PIN)&&retry<100)//DHT11拉低后会再次拉高40~80us
  {
    retry++;
    vDelayUs(1);
  };
  if(retry>=100)return 1;	    
  return 0;
}

//从DHT11读取一个位
//返回值：1/0
uint8_t vDht11ReadBit(void) 			 
{
  uint8_t retry=0;
  while(GPIO_ReadInputDataBit(DHT11_GPIO_PORT,DHT11_GPIO_PIN)&&retry<100)//等待变为低电平
  {
    retry++;
    vDelayUs(1);
  }
  retry=0;
  while(!GPIO_ReadInputDataBit(DHT11_GPIO_PORT,DHT11_GPIO_PIN)&&retry<100)//等待变高电平
  {
    retry++;
    vDelayUs(1);
  }
  vDelayUs(40);//等待40us
  if(GPIO_ReadInputDataBit(DHT11_GPIO_PORT,DHT11_GPIO_PIN))return 1;
  else return 0;		   
}

//从DHT11读取一个字节
//返回值：读到的数据
uint8_t vDht11ReadByte(void)    
{        
  uint8_t i,dat;
  dat=0;
  for (i=0;i<8;i++) 
  {
    dat<<=1; 
    dat|=vDht11ReadBit();
  }						    
  return dat;
}

//从DHT11读取一次数据
//temp:温度值(范围:0~50°)
//humi:湿度值(范围:20%~90%)
//返回值：0,正常;1,读取失败
uint8_t vDht11ReadData(uint8_t *pucTemp,uint8_t *pucHumi)    
{        
  uint8_t ucBuffer[5];
  uint8_t i;
  vDht11Rst();
  if(vDht11Check()==0)
  {
    for(i=0;i<5;i++)//读取40位数据
    {
      ucBuffer[i]=vDht11ReadByte();
    }
    if((ucBuffer[0]+ucBuffer[1]+ucBuffer[2]+ucBuffer[3])==ucBuffer[4])
    {
      *pucHumi=ucBuffer[0];
      *pucTemp=ucBuffer[2];
    }
  }
  else return 1;
  return 0;	    
}

//初始化DHT11的IO口 DQ 同时检测DHT11的存在
//返回1:不存在
//返回0:存在    	 
uint8_t vDht11Init(void)
{	 
  GPIO_InitTypeDef  GPIO_InitStructure;	
  RCC_APB2PeriphClockCmd(DHT11_GPIO_CLK, ENABLE);	 //使能PA端口时钟
  GPIO_InitStructure.GPIO_Pin = DHT11_GPIO_PIN;				 //PG11端口配置
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(DHT11_GPIO_PORT, &GPIO_InitStructure);				 //初始化IO口
  GPIO_SetBits(DHT11_GPIO_PORT,DHT11_GPIO_PIN);						 //PG11 输出高

  vDht11Rst();  //复位DHT11
  return vDht11Check();//等待DHT11的回应
} 

void vDht11Mode(uint8_t ucMode)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  if(ucMode)
  {
    GPIO_InitStructure.GPIO_Pin = DHT11_GPIO_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  }
  else
  {
    GPIO_InitStructure.GPIO_Pin =  DHT11_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  }
  GPIO_Init(DHT11_GPIO_PORT, &GPIO_InitStructure);
}
