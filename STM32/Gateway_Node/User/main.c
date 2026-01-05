#include "stm32f10x.h"                  // Device header
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "Delay.h"
#include "USART.h"
#include "LED.h"
#include "ESP8266.h"
#include "NetFIFO.h"

/* 创建一个新任务，需要创建任务句柄,任务句柄与任务函数一一对应。 */
TaskHandle_t xTaskStateLedHdlr;
TaskHandle_t xTaskBufferRxHdlr;
TaskHandle_t xTaskApConnectHdlr;

/* 创建队列句柄 */
QueueHandle_t xQueueUsart1IrqHdlr;
QueueHandle_t xQueueUsart2IrqHdlr;
QueueHandle_t xQueueUsart3IrqHdlr;

/* 创建二值信号量句柄 */
SemaphoreHandle_t xSemWifiRetOkHdlr;
SemaphoreHandle_t xSemWifiRetErrHdlr;

/**
  * @brief  状态指示LED任务         
  * @note   通过控制PC13引脚的LED灯，实现系统状态的指示功能。
  * @param  *pvParameters 任务参数，若没有特定的参数则设置为空指针
  * @retval None
  */
/* 创建任务 */
void vTaskStateLed(void *pvParameters)
{
    while(1)
    {
        vPc13LedOn();
        vTaskDelay(1000);
        vPc13LedOff();
        vTaskDelay(1000);
    }
}


void vTaskApConnection(void *pvParameters)
{
    while (1)
    {
        macESP8266_CH_ENABLE ();
        vEsp8266AtTest ();
        bEsp8266NetModeChoose ( STA );
        while ( ! bEsp8266JoinAp ( macUser_ESP8266_ApSsid, macUser_ESP8266_ApPwd ) );
        vTaskDelete(NULL);
    }
}


/**
  * @brief  创建所有任务列表         
  * @note   在此函数中创建所有需要的任务，并为每个任务分配适当的堆栈大小和优先级。
  * @param  None
  * @retval None
  */
void vCreateTasksList(void)
{
    xTaskCreate(
               (TaskFunction_t        ) vTaskStateLed,
               (char *                ) "TaskName_StateLed", 
               (configSTACK_DEPTH_TYPE) 256,
               (void *                ) NULL, 
               (UBaseType_t           ) 2,
               (TaskHandle_t *        ) &xTaskStateLedHdlr);
    xTaskCreate(
               (TaskFunction_t        ) vTaskApConnection,
               (char *                ) "TaskName_WifiConnectToAP", 
               (configSTACK_DEPTH_TYPE) 512,
               (void *                ) NULL, 
               (UBaseType_t           ) 2,
               (TaskHandle_t *        ) &xTaskApConnectHdlr);
}

/**
  * @brief  创建所有队列列表         
  * @note   在此函数中创建所有需要的队列，并为每个队列分配适当的长度和项大小。
  * @param  None
  * @retval None
  */
void vCreateQueuesList(void)
{
    #if (ENABLE_FREERTOS_API_QUEUE_USART1_IRQ == 1)
        xQueueUsart1IrqHdlr = xQueueCreate(
                                      (UBaseType_t) 64,
                                      (UBaseType_t) sizeof(uint8_t *));
    #endif
    #if (ENABLE_FREERTOS_API_QUEUE_USART2_IRQ == 1)
        xQueueUsart2IrqHdlr = xQueueCreate(
                                      (UBaseType_t) 1024,
                                      (UBaseType_t) sizeof(char *));
    #endif
    #if (ENABLE_FREERTOS_API_QUEUE_USART3_IRQ == 1)
        xQueueUsart3IrqHdlr = xQueueCreate(
                                      (UBaseType_t) 64,
                                      (UBaseType_t) sizeof(uint8_t *));
    #endif
    
}

/**
  * @brief  创建所有信号量列表         
  * @note   在此函数中创建所有需要的信号量。
  * @param  None
  * @retval None
  */
void vCreateSemaphoresList(void)
{
    xSemWifiRetOkHdlr  = xSemaphoreCreateBinary();
    xSemWifiRetErrHdlr = xSemaphoreCreateBinary();
}

/**
  * @brief  主函数         
  * @note   初始化系统各个模块，创建任务、队列和信号量，并启动调度器。
  * @param  None
  * @retval int
  */
int main(void)
{
    vUsartInit(USART2, 115200);
    vUsartInit(USART3, 115200);
    vDelayInit();
    vPc13LedInit();
    vCreateTasksList();
    vCreateQueuesList();
    vCreateSemaphoresList();
    vTaskStartScheduler();
    while(1)
    {

    }
}
