#include "stm32f10x.h"                  // Device header
#include "FreeRTOS.h"
#include "task.h"

/* 创建一个新任务，需要创建任务句柄,任务句柄与任务函数一一对应。 */
TaskHandle_t xSampleTaskHandler;

/* 创建任务 */
void vSampleTask(void *pvParameters)
{
    while(1)
	  {
    
    }
}

void vCreateTasksList(void)
{
    xTaskCreate(
               (TaskFunction_t        ) vSampleTask,
               (char *                ) "TaskName_SampleTask", 
               (configSTACK_DEPTH_TYPE) 256,
               (void *                ) NULL, 
               (UBaseType_t           ) 2,
               (TaskHandle_t *        ) &xSampleTaskHandler);
}

void vCreateQueuesList(void)
{

}

void vCreateSemaphoresList(void)
{
   
}

int main(void)
{
    vCreateTasksList();
    vCreateQueuesList();
    vCreateSemaphoresList();
    vTaskStartScheduler();
    while(1)
    {

    }
}
