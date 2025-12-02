/*
 * FreeRTOS V202212.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE. 
 *
 * See http://www.freertos.org/a00110.html
 *----------------------------------------------------------*/

#define configUSE_PREEMPTION		                     1
#define configUSE_IDLE_HOOK		                       0
#define configUSE_TICK_HOOK		                       0
#define configCPU_CLOCK_HZ		                       ( ( unsigned long ) 72000000 )	
#define configTICK_RATE_HZ		                       ( ( TickType_t ) 1000 )
#define configMAX_PRIORITIES		                     ( 5 )
#define configMINIMAL_STACK_SIZE		                 ( ( unsigned short ) 128 )
#define configTOTAL_HEAP_SIZE		                     ( ( size_t ) ( 17 * 1024 ) )
#define configMAX_TASK_NAME_LEN		                   ( 16 )
#define configUSE_TRACE_FACILITY		                 0
/* 系统节拍计数器变量数据类型，1表示为16位无符号整形，0表示为32位无符号整形 */
#define configUSE_16_BIT_TICKS		                   0
/* 1、空闲任务放弃CPU使用权,给其他同优先级的用户任务 2、空闲优先级和其他优先级相同。避免2，多使用1 */
#define configIDLE_SHOULD_YIELD		                   1
/* 后加的部分 */
/* 时间片调度，当优先级相同时执行 */
#define configUSE_TIME_SLICING                       1
/* 队列。为1开启，为0关闭 */
#define configUSE_QUEUE_SETS                         0
/* 开启任务通知功能，默认开启 */
#define configUSE_TASK_NOTIFICATIONS                 1
/* 互斥信号量开关 */
#define configUSE_MUTEXES                            0
/* 使用递归互斥信号量 */
#define configUSE_RECURSIVE_MUTEXES                  0
/* 为1时使用计数信号量 */
#define configUSE_COUNTING_SEMAPHORES                0
/* 设置可以注册的信号量和消息队列个数 */
#define configQUEUE_REGISTRY_SIZE                    10                                                                                  
#define configUSE_APPLICATION_TASK_TAG               0
/* FreeRTOS与内存申请有关配置选项 */
/* 支持动态内存申请 */
#define configSUPPORT_DYNAMIC_ALLOCATION             1    
/* 支持静态内存 */
#define configSUPPORT_STATIC_ALLOCATION              0					
/*
 * 大于0时启用堆栈溢出检测功能，如果使用此功能 
 * 用户必须提供一个栈溢出钩子函数，如果使用的话
 * 此值可以为1或者2，因为有两种栈溢出检测方法 */
#define configCHECK_FOR_STACK_OVERFLOW		           0   
/* FreeRTOS与运行时间和任务状态收集有关的配置选项 */
/* 启用运行时间统计功能 */
#define configGENERATE_RUN_TIME_STATS	               0

/* Co-routine definitions. */
/* 启用协程，启用协程以后必须添加文件croutine.c */
#define configUSE_CO_ROUTINES 		                   0
/* 协程的有效优先级数目 */
#define configMAX_CO_ROUTINE_PRIORITIES              ( 2 )

/* 后加的部分 */
/* FreeRTOS与软件定时器有关的配置选项 */
/* 启用软件定时器 */
#define configUSE_TIMERS                             0                              
/* 软件定时器优先级 */
#define configTIMER_TASK_PRIORITY                    (configMAX_PRIORITIES-1)        
/* 软件定时器队列长度 */
#define configTIMER_QUEUE_LENGTH                     10                               
/* 软件定时器任务堆栈大小 */
#define configTIMER_TASK_STACK_DEPTH                 (configMINIMAL_STACK_SIZE*2)
/*
 * 某些运行FreeRTOs的硬件有两种方法选择下一个要执行的任务:
 *
 * 通用方法和特定于硬件的方法（以下简称"特殊方法")。
 * 通用方法:
 * 1.configUSE_PORT_OPTIMISED_TASK_SELECTION为О或者硬件不支持这种特殊方法。
 * 2.可以用于所有FreeRTOS支持的硬件
 * 3.完全用c实现,效率略低于特殊方法。
 * 4.不强制要求限制最大可用优先级数目

 * 特殊方法:
 * 1.必须将configUSE_PORT_OPTIMISED_TASK_SELECTION设置为1。
 * 2.依赖一个或多个特定架构的汇编指令（一般是类似计算前导零[CLZ]指令）。
 * 3.比通用方法更高效
 * 4.一般强制限定最大可用优先级数目为32
 * 一般是硬件计算前导零指令，如果所使用的，MCU没有这些硬件指令的话此宏应该设置为0 !
 */
#define configUSE_PORT_OPTIMISED_TASK SELECTION      1
/*
 * configUSE_TICKLESS_IDLE  

 * 置1:使能低功耗tickless模式;置0:保持系统节拍(tick）中断一直运行
 * 假设开启低功耗的话可能会导致下载出现问题，因为程序在睡眠中,可用以下办法解决
 * 下载方法:
 * 1.将开发版正常连接好
 * 2.按住复位按键,点击下载瞬间松开复位按键
 * 

 * 1.通过跳线帽将BO0T 0接高电平(3.3v)
 * 2.重新上电,下载
 * 1.使用FlyMcu擦除一下芯片,然后进行下载STMISP ->清除芯片(z)
 * 
 */
#define configUSE_TICKLESS_IDLE                      0
/* 取代原有的中断函数 */
#define xPortPendSVHandler                           PendSV_Handler 
#define vPortSVCHandler                              SVC_Handler
#define INCLUDE_xTaskGetSchedulerState               1

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskPrioritySet                     1
#define INCLUDE_uxTaskPriorityGet		                 1
#define INCLUDE_vTaskDelete				                   1
#define INCLUDE_vTaskCleanUpResources	               0
#define INCLUDE_vTaskSuspend			                   1
#define INCLUDE_vTaskDelayUntil			                 1
#define INCLUDE_vTaskDelay				                   1
/* 后加的部分 */
#define INCLUDE_eTaskGetState                        1
#define INCLUDE_xTimerPendFunctionCall               0
#define INCLUDE_xTaskGetSchedulerState               1
/* FreeRTOS与中断有关的配置选项 */
#ifdef __NVIC_PRIO_BITS
	#define configPRIO_BITS                            __NVIC_PRIO_BITS
#else
	#define configPRIO_BITS                            4                  
#endif
/* 中断最低优先级 */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY      15
/* 系统可管理的最高中断优先级 */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 5
/* This is the raw value as per the Cortex-M3 NVIC.  Values can be 255
(lowest) to 0 (1?) (highest). */
#define configKERNEL_INTERRUPT_PRIORITY              255
/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	       191 /* equivalent to 0xb0, or priority 11. */


/* This is the value being used as per the ST library which permits 16
priority values, 0 to 15.  This must correspond to the
configKERNEL_INTERRUPT_PRIORITY setting.  Here 15 corresponds to the lowest
NVIC value of 255. */
#define configLIBRARY_KERNEL_INTERRUPT_PRIORITY	     15

#endif /* FREERTOS_CONFIG_H */
