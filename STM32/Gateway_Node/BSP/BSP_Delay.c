#include "BSP_Delay.h"

#if USE_SYS_TICK

/**
 * @brief  初始化延迟函数
 * @note   延时函数初始化。SYSTICK的时钟固定为AHB时钟的1/8。SYSCLK:系统时钟频率
 * @param  None
 * @retval None
 */
void delayInit(void)
{
    uint8_t systemClock;
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
    facInit.facUs = systemClock / 8;
    facInit.facMs = (uint16_t)facInit.facUs * 1000;
}

/**
  * @brief  微秒级延时函数
  * @note   最大延时16777215us
  * @param  us: 延时的微秒数
  * @retval None
  */
void delayUs(uint32_t us)
{
    uint32_t temp;	    	 
    SysTick->LOAD = us * facInit.facUs;                                 // 时间加载
    SysTick->VAL = 0x00;                                                // 清空计数器
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;                           // 开始倒数	  
    do
    {
        temp = SysTick->CTRL;
    } while((temp & 0x01) && !(temp & (1 << 16)));                      // 等待时间到达   
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;                          // 关闭计数器
    SysTick->VAL = 0x00;                                                // 清空计数器	 	
}

/**
  * @brief  毫秒级延时函数
  * @note   由于Systick定时器为24位定时器，其装载的最大值为0xFFFFFF，也就是16777215，如果加载的值超过0xFFFFFF，则实际加载的值是取模后的值，导致延时时间缩短。
  *         所以超过1.864s的延时会导致计数器溢出，从而导致定时为xms-16777215的结果。
  * @note   如下函数会判断延时是否超过最大延时，若超过则分成多次1000ms处理。
  * @param  None
  * @retval None
  */
void delayMs(uint16_t ms)
{
    uint32_t temp;
    while (ms)
    {
        uint16_t current = (ms > 1000) ? 1000 : ms;
        ms -= current;

        SysTick->LOAD = (uint32_t)current * facInit.facMs;
        SysTick->VAL = 0x00;
        SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
        do
        {
            temp = SysTick->CTRL;
        } while ((temp & 0x01) && !(temp & (1 << 16)));
        SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
        SysTick->VAL = 0x00;
    }   
}

#elif USE_DWT

/**
  * @brief  DWT延时函数初始化
  * @note   DWT延时精度高，适合短时间延时
  * @param  None
  * @retval None
  */
void delayInit(void)
{
    DEMCR |= (uint32_t)TRCENA;                                          // 使能DWT外设
    DWT_CYCCNT = (uint32_t)0u;                                          // CYCCNT计数器清零
    DWT_CTRL |= (uint32_t)DWT_CTRL_CYCCNTENA;                           // 使能CYCCNT计数器    
}

/**
  * @brief  微秒级延时函数
  * @note   最大延时10000us
  * @param  us: 延时的微秒数
  * @retval None
  */
void delayUs(uint32_t us)
{
    if(us > 10000) us = 10000;
    uint32_t ticksStart, ticksEnd, ticksDelay;
    ticksStart = DWT_CYCCNT;
    ticksDelay = (us * (SystemCoreClock / (1000000)));                  // 将微秒数换算成滴答数
    ticksEnd = ticksStart + ticksDelay;
    /* 当ticksEnd没有溢出时 */
    if (ticksEnd >= ticksStart)                            
    {
        if(DWT_CYCCNT > ticksStart)                                     // DWT_CYCCNT在上述计算的这段时间中没有溢出               
        {
            while(DWT_CYCCNT < ticksEnd);
        }
        /* DWT_CYCCNT溢出时 */
        else                                                    
        {
            return;                                                     // 已经超时，直接退出
        }
    }
    /* 当ticksEnd溢出时 */
    else                                                        
    {
        /* DWT_CYCCNT在上述计算的这段时间中没有溢出时 */
        if(DWT_CYCCNT > ticksStart)                           
        {
            while(DWT_CYCCNT > ticksEnd);                               // 等待DWT_CYCCNT溢出
        }
        while(DWT_CYCCNT < ticksEnd);                                   // 等待溢出后的DWT_CYCCNT到达ticksEnd
    }
}

/**
  * @brief  毫秒级延时函数
  * @note   最大延时65535ms
  * @param  ms: 延时的毫秒数
  * @retval None
  */
void delayMs(uint16_t ms)
{
    for(uint16_t i = 0; i < ms; i++)
    {
        delayUs(1000);
    }
}

#else
#endif
