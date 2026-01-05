#include "DHT11.h"

/**
  * @brief  DHT11工作模式设置函数
  * @note   设置DHT11的IO口为输入或输出模式。
  * @param  ucMode 模式选择，1为输出模式，0为输入模式
  * @retval None
  */
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

/**
  * @brief  DHT11复位函数
  * @note   主机向DHT11发送复位信号，准备读取数据
  * @param  None
  * @retval None
  */
void vDht11Rst(void)
{                 
    vDht11Mode(DHT11_MODE_OUT);                                               // SET OUTPUT
    DHT11_Low;                                                                // 拉低DQ
    vDelayMs(20);                                                             // 主机拉低18~30ms
    DHT11_High;                                                               // DQ=1 
    vDelayUs(13);                                                             // 主机拉高10~35us
}

/**
  * @brief  DHT11检查函数
  * @note   检测DHT11的存在与否，通过读取响应信号判断。
  * @param  None
  * @retval 0:存在 1:不存在 
  */
uint8_t vDht11Check(void)  
{   
    uint8_t retry=0;
    vDht11Mode(DHT11_MODE_IN);//SET INPUT
    while (GPIO_ReadInputDataBit(DHT11_GPIO_PORT, DHT11_GPIO_PIN)&&retry<100)  // DHT11会拉低40~80us
    {
        retry++;
        vDelayUs(1);
    };
    if(retry>=100)
        return 1;
    else retry=0;
    while (!GPIO_ReadInputDataBit(DHT11_GPIO_PORT, DHT11_GPIO_PIN)&&retry<100) // DHT11拉低后会再次拉高40~80us
    {
        retry++;
        vDelayUs(1);
    };
    if(retry>=100)
        return 1;
    return 0;
}

/**
  * @brief  DHT11初始化函数
  * @note   初始化DHT11的IO口，并检测DHT11的存在。
  * @param  None
  * @retval 0:存在;1:不存在
  */
uint8_t vDht11Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;	
    RCC_APB2PeriphClockCmd(DHT11_GPIO_CLK, ENABLE);                           // 使能PA端口时钟
    GPIO_InitStructure.GPIO_Pin = DHT11_GPIO_PIN;                             // PG11端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;                          // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	  GPIO_Init(DHT11_GPIO_PORT, &GPIO_InitStructure);                          // 初始化IO口
    GPIO_SetBits(DHT11_GPIO_PORT, DHT11_GPIO_PIN);                             // PG11 输出高

    vDht11Rst();                                                              // 复位DHT11
    return vDht11Check();                                                     // 等待DHT11的回应
} 

/**
  * @brief  DHT11读取位函数
  * @note   从DHT11读取一个数据位，通过检测高电平持续时间判断是0还是1。
  * @param  None
  * @retval 读取到的位值，1或0
  */
uint8_t vDht11ReadBit(void)
{
    uint8_t retry=0;
    while(GPIO_ReadInputDataBit(DHT11_GPIO_PORT, DHT11_GPIO_PIN)&&retry<100)     // 等待变为低电平
    {
        retry++;
        vDelayUs(1);
    }
    retry=0;
    while(!GPIO_ReadInputDataBit(DHT11_GPIO_PORT, DHT11_GPIO_PIN)&&retry<100)    // 等待变高电平
    {
        retry++;
        vDelayUs(1);
    }
    vDelayUs(40);//等待40us
    if(GPIO_ReadInputDataBit(DHT11_GPIO_PORT, DHT11_GPIO_PIN))
        return 1;
    else return 0;
}

/**
  * @brief  DHT11读取字节函数
  * @note   从DHT11读取一个字节数据，通过连续读取8个位并组合成字节。
  * @param  None
  * @retval 读取到的字节数据
  */
uint8_t vDht11ReadByte(void)    
{        
    uint8_t i, dat;
    dat=0;
    for (i=0;i<8;i++) 
    {
        dat<<=1; 
        dat|=vDht11ReadBit();
    }
    return dat;
}

/** @brief  DHT11读取数据函数
  * @note   从DHT11读取温度和湿度数据，并进行校验。
  * @param  *pucTemp 指向存储温度数据的变量指针
  * @param  *pucHumi 指向存储湿度数据的变量指针
  * @retval 0:正常;1:读取失败
  */
uint8_t vDht11ReadData(uint8_t *pucTemp, uint8_t *pucHumi)    
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
