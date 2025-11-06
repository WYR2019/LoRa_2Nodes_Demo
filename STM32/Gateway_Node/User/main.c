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

int main(void)
{
  xTaskCreate(vSampleTask, "SampleTask", 128, NULL, 2, &xSampleTaskHandler);
  vTaskStartScheduler();
  while(1)
  {
    
  }
}
