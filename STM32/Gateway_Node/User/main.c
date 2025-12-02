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

/* 待验证的改动：合并串口。 */
/* 创建一个新任务，需要创建任务句柄,任务句柄与任务函数一一对应。 */
TaskHandle_t xTaskStateLedHdlr;
TaskHandle_t xTaskWifiConnectHdlr;

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

/**
  * @brief  连接WiFi任务         
  * @note   利用FreeRTOS中断消息队列机制，接收ESP8266的响应数据，并将响应数据存到内置的FIFO缓冲区内。在再判断数据是否符合要求。
  * @note   问题：将接收消息队列函数和处理响应数据函数混到一块了，造成了代码的冗余。
  * @param  *pvParameters 任务参数，若没有特定的参数则设置为空指针
  * @retval None
  */
void vTaskWifiConnection(void *pvParameters)
{
   NetFifoBuffer_t espBuffer;
   vNetBufferInit(&espBuffer);
   uint8_t ucRetvalQueueWifiSta = 0;
   uint8_t ucRetvalQueueByte = 0;
   while (1)
   {
       /* code */
       ucEsp8266SetMode(1);
       TickType_t startTime = xTaskGetTickCount();
       while ((xTaskGetTickCount() - startTime) < pdMS_TO_TICKS(3000))
       {
            ucRetvalQueueWifiSta = xQueueReceive(xQueueUsart2IrqHdlr, 
                                             &ucRetvalQueueByte, 
                                             pdMS_TO_TICKS(100));
            if (ucRetvalQueueWifiSta == pdTRUE)
            {
                // 将接收到的字节处理到缓冲区
                vNetBufferWrite(&espBuffer, (char)ucRetvalQueueByte);
                // 处理响应数据
                uint8_t ucResponseStatus = ucEsp8266ResponseHandler(&espBuffer);
                if (strstr(espBuffer.cResponseBuffer, "ready") != NULL)
                {
                    vUsartSendString(USART3, "WiFi Reset Successfully!\r\n");
                    vUsartSendString(USART3, espBuffer.cResponseBuffer);
                    vTaskDelay(1000);
                    break;
                } else if (strstr(espBuffer.cResponseBuffer, "OK") != NULL)
                {
                    /* code */
                    vUsartSendString(USART3, "Set WiFi Mode Successfully!\r\n");
                    vUsartSendString(USART3, espBuffer.cResponseBuffer);
                } else if (strstr(espBuffer.cResponseBuffer, "ERROR") != NULL || 
                         strstr(espBuffer.cResponseBuffer, "FAIL") != NULL)
                {
                    vUsartSendString(USART3, "Error!\r\n");
                    vUsartSendString(USART3, espBuffer.cResponseBuffer);
                    vUsartSendString(USART3, "\r\n");
                    break;
                }
            }
        }
        vTaskDelete(NULL);
    }
}

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
               (TaskFunction_t        ) vTaskWifiConnection,
               (char *                ) "TaskName_WifiConnectToAP", 
               (configSTACK_DEPTH_TYPE) 512,
               (void *                ) NULL, 
               (UBaseType_t           ) 2,
               (TaskHandle_t *        ) &xTaskWifiConnectHdlr);
}

void vCreateQueuesList(void)
{
    xQueueUsart1IrqHdlr = xQueueCreate(
                                      (UBaseType_t) 64,
                                      (UBaseType_t) sizeof(uint8_t *));
    xQueueUsart2IrqHdlr = xQueueCreate(
                                      (UBaseType_t) 1024,
                                      (UBaseType_t) sizeof(char *));
    xQueueUsart3IrqHdlr = xQueueCreate(
                                      (UBaseType_t) 64,
                                      (UBaseType_t) sizeof(uint8_t *));
    if (xQueueUsart2IrqHdlr == NULL)
    {
        /* code */
        vUsartSendString(USART1, "Queue Init Failed.\r\n");
        vTaskDelay(1000);
    }
}

void vCreateSemaphoresList(void)
{
    xSemWifiRetOkHdlr  = xSemaphoreCreateBinary();
    xSemWifiRetErrHdlr = xSemaphoreCreateBinary();
}

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
