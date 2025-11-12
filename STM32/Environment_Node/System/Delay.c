#include "Delay.h"

#if MODE == USE_SYS_TICK

//初始化延迟函数
//SYSTICK的时钟固定为AHB时钟的1/8
//SYSCLK:系统时钟频率
void vDelayInit(void)
{
    uint8_t ucSYSCLK;
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); 
    xUcFacInit.ucFacUs=ucSYSCLK/8;					
    xUcFacInit.usFacMs=(uint16_t)xUcFacInit.ucFacUs*1000;				   
}								    

//延时nus
//nus为要延时的us数.		    								   
void vDelayUs(uint32_t ulNus)
{
    uint32_t ulTemp;	    	 
    SysTick->LOAD=ulNus*xUcFacInit.ucFacUs; 					      //时间加载	  		 
    SysTick->VAL=0x00;        					                    //清空计数器
    SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;                //开始倒数	  
    do
    {
        ulTemp=SysTick->CTRL;
    } while((ulTemp&0x01)&&!(ulTemp&(1<<16)));		            //等待时间到达   
    SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	              //关闭计数器
    SysTick->VAL =0x00;      					                      //清空计数器	 	
}

//延时nms
//注意nms的范围
//SysTick->LOAD为24位寄存器,所以,最大延时为:
//nms<=0xffffff*8*1000/SYSCLK
//SYSCLK单位为Hz,nms单位为ms
//对72M条件下,nms<=1864

/**
  * @brief  毫秒级延时函数
  * @note   由于Systick定时器为24位定时器，其装载的最大值为0xFFFFFF，也就是16777215，如果加载的值超过0xFFFFFF，则实际加载的值是取模后的值，导致延时时间缩短。
  *         所以超过1.864s的延时会导致计数器溢出，从而导致定时为xms-16777215的结果。
  * @note   如下函数会判断延时是否超过最大延时，若超过则分成多次1000ms处理。
  * @param  None
  * @retval None
  */
void vDelayMs(uint16_t usNms)
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
        SysTick->VAL = 0x00;
    }   
}

#elif MODE == USE_DWT

void vDelayInit(void)
{
    //使能DWT外设
    DEMCR |= (uint32_t)TRCENA;

    //DWT CYCCNT寄存器计数清0
    DWT_CYCCNT = (uint32_t)0u;

    //使能Cortex-M3 DWT CYCCNT寄存器
    DWT_CTRL |= (uint32_t)DWT_CTRL_CYCCNTENA;
}

// 微秒延时
void vDelayUs(uint32_t ulNus)
{
    if(ulNus > 10000) ulNus = 10000;

    uint32_t ulTicksStart, ulTicksEnd, ulTicksDelay;

    ulTicksStart = DWT_CYCCNT;
    ulTicksDelay = ( ulNus * ( SystemCoreClock / (1000000) ) ); // 将微秒数换算成滴答数
    ulTicksEnd = ulTicksStart + ulTicksDelay;

    // ulTicksEnd没有溢出
    if ( ulTicksEnd >= ulTicksStart )
    {
        // DWT_CYCCNT在上述计算的这段时间中没有溢出
        if(DWT_CYCCNT > ulTicksStart)
        {
            while( DWT_CYCCNT < ulTicksEnd );
        }
        // DWT_CYCCNT溢出
        else
        {
            // 已经超时，直接退出
            return;
        }
    }
    else // ulTicksEnd溢出
    {
        // DWT_CYCCNT在上述计算的这段时间中没有溢出
        if(DWT_CYCCNT > ulTicksStart)
        {
            // 等待DWT_CYCCNT的值溢出
            while( DWT_CYCCNT > ulTicksEnd );
        }
        // 等待溢出后的DWT_CYCCNT到达ulTicksEnd
        while( DWT_CYCCNT < ulTicksEnd );
    }
}

void vDelayMs(uint16_t usNms)
{
    for(uint16_t i = 0; i < usNms; i++)
    {
        // delay 1 ms
        vDelayUs(1000);
    }
}

#else
#endif
