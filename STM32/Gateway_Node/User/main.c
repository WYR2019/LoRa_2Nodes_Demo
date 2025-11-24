#include "stm32f10x.h"                  // Device header
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "Delay.h"
#include "USART.h"
#include "LED.h"

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

/* 创建任务 */
void vTaskStateLed(void *pvParameters)
{
    while(1)
    {
        vPc13LedOn();
        vTaskDelay(1000);
        vUsartPrintf(USART3, "USART3 Printf Test %d\r\n", 123);
        vPc13LedOff();
        vTaskDelay(1000);
    }
}

void vTaskWifiConnection(void *pvParameters)
{
    uint8_t ucRetvalQueueWifiSt = 0;
    uint8_t ucBufferQueueRec = 0;
    vUsartPrintf(USART2, "AT+RST\r\n");
    vTaskDelay(1000);
    vUsartPrintf(USART2, "AT+CWMODE=1\r\n");
    vTaskDelay(1000);
    /* 添加转义字符 */
    vUsartPrintf(USART2, "AT+CWJAP=\"Exploration\",\"IPSK25em.\"\r\n");
    while (1)
    {
        
        ucRetvalQueueWifiSt = xQueueReceive(xQueueUsart2IrqHdlr, &ucBufferQueueRec, pdMS_TO_TICKS(30));
        /* code */
        if (ucRetvalQueueWifiSt == pdTRUE)
        {
            /* code */
            vUsartPrintf(USART3, (char *)&ucBufferQueueRec);
        } 
        else
        {
            vUsartSendString(USART3, "Queue Receive failed!\r\n");
            vTaskDelay(300);
        }
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
                                      (UBaseType_t) 64,
                                      (UBaseType_t) sizeof(uint8_t *));
    xQueueUsart3IrqHdlr = xQueueCreate(
                                      (UBaseType_t) 64,
                                      (UBaseType_t) sizeof(uint8_t *));
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
