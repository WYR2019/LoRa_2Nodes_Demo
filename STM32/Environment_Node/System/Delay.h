#ifndef __DELAY_H__
#define __DELAY_H__                            

#include "stm32f10x_conf.h"
#include "stm32f10x.h"

#define MODE                      USE_DWT

#define USE_SYS_TICK              0
#define USE_DWT                   1

// 0xE000EDFC DEMCR RW Debug Exception and Monitor Control Register.
#define DEMCR                     ( *(__IO uint32_t *)0xE000EDFC )
#define TRCENA                    ( 0x01 << 24)                         // DEMCR的DWT使能位
#define  DBGMCU_CR                *(__IO uint32_t *)0xE0042004          // MCU调试模块控制寄存器，详细内容参考《stm32中文参考手册》调试支持（DBG）章节，747页

// 0xE0001000 DWT_CTRL RW The Debug Watchpoint and Trace (DWT) unit
#define DWT_CTRL                  ( *(__IO uint32_t *)0xE0001000 )
#define DWT_CTRL_CYCCNTENA        ( 0x01 << 0 )                         // DWT的SYCCNT使能位

// 0xE0001004 DWT_CYCCNT RW Cycle Count register,
#define DWT_CYCCNT                ( *(__IO uint32_t *)0xE0001004 )      // 显示或设置处理器的周期计数值

//#define DWT_DELAY_mS(mSec)      DWT_DELAY_uS(mSec*1000)

typedef struct {
  uint8_t ucFacUs;
  int16_t usFacMs;
} UcFacInit_t;

static UcFacInit_t xUcFacInit = {
  .ucFacUs = 0,
  .usFacMs = 0
};

void vDelayInit(void);
void vDelayUs(uint32_t ulNus);
void vDelayMs(uint16_t usNms);

#endif
