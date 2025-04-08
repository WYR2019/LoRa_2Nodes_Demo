//添加一个防止头文件重复包含的代码if not define，如果没有定义该字符串，那么就先定义一个
#ifndef __LED_H__
#define __LED_H__
//变量声明
void LED_Init(void);
void LED1_ON(void);
void LED1_OFF(void);
void LED2_ON(void);
void LED2_OFF(void);
void LED1_Turn(void);
void LED2_Turn(void);
#endif
