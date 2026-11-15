#include "stm32f10x.h"                  // Device header
#include "LED.h"
#include "Relay.h"
#include "USART.h"
#include "DHT11.h"
#include "LoRa.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* 在创建任务前，需要创建一个任务句柄，每个任务句柄与任务一一对应。 */
TaskHandle_t taskWorkStateLedHdlr;
TaskHandle_t taskDht11Hdlr;
TaskHandle_t taskLoRaSendHdlr;
TaskHandle_t taskLoRaReceiveHdlr;
TaskHandle_t taskTraceHdlr;

/* 创建队列句柄 */
QueueHandle_t queueTempHdlr;
QueueHandle_t queueHumiHdlr;
#if ENABLE_FREERTOS_API_QUEUE_USART1_IRQ
    QueueHandle_t xQueueUsart1IrqHdlr;
#endif
#if ENABLE_FREERTOS_API_QUEUE_USART2_IRQ
    QueueHandle_t xQueueUsart2IrqHdlr;
#endif
#if ENABLE_FREERTOS_API_QUEUE_USART3_IRQ
    QueueHandle_t xQueueUsart3IrqHdlr;
#endif

/* 创建二值信号量句柄 */
SemaphoreHandle_t semLedOnHdlr;
SemaphoreHandle_t semLedOffHdlr;
SemaphoreHandle_t semFanOnHdlr;
SemaphoreHandle_t semFanOffHdlr;
SemaphoreHandle_t semHumidifierOnHdlr;
SemaphoreHandle_t semHumidifierOffHdlr;

/**
  * @brief  LED状态指示任务函数
  * @note   通过LED的闪烁状态指示系统运行状态。
  * @param  *parameters 任务参数，未使用，传入NULL。
  * @retval None
  */
void taskWorkStateLed(void *parameters)
{
    while(1)
    {
        pc13LedOn();
        vTaskDelay(1000);
        pc13LedOff();
        vTaskDelay(1000);
    }
}

/**
  * @brief  DHT11数据读取任务函数
  * @note   定期读取DHT11传感器的数据，并将温度和湿度数据发送到相应的队列。
  * @param  *parameters 任务参数，未使用，传入NULL。
  * @retval None
  */
void taskDht11(void *parameters)
{
    DHT11Data_t dht11Data;
    /* 数据存储数组。要求与队列参数设置大小一一对应。 */
    uint8_t putTemp = 0, putHumi = 0;
    while (1)
    {
        /* 读取成功处理 */
        if (dht11ReadData(&dht11Data.temp, &dht11Data.humi) == 0)
        {
            putTemp = dht11Data.temp;
            putHumi = dht11Data.humi;
            /* 写队列。第一个参数是队列句柄，第二个是写入队列的地址（如数组），第三个是因队列满造成阻塞时的等待时长，也就是无法写入数据时的等待时长，以操作系统默认时钟节拍为准。 */
            xQueueSend(queueTempHdlr, &putTemp, pdMS_TO_TICKS(10));
            xQueueSend(queueHumiHdlr, &putHumi, pdMS_TO_TICKS(10));
        }
    }
}

/**
  * @brief  LoRa数据发送任务函数
  * @note   从温湿度队列接收数据，并通过LoRa模块发送到网关。同时处理执行器的控制信号。
  * @param  *parameters 任务参数，未使用，传入NULL。
  * @retval None
  */
void taskLoRaSend(void *parameters)
{
    /* 定义一个返回值判断是否接收成功 */
    BaseType_t getTemp = 0, getHumi = 0;
    uint8_t temp = 0, humi = 0;
    uint8_t commandLed = LORA_EXECUTOR_COMMAND_OFF;
    uint8_t commandFan = LORA_EXECUTOR_COMMAND_OFF;
    uint8_t commandHumidifier = LORA_EXECUTOR_COMMAND_OFF;
    while (1)
    {
        /* 接收队列：队列句柄、传输转存的目标变量或数组和未接收到数据的等待时长，分3种情况，分别为0、0~portMAX_DELAY和portMAX_DELAY三种，分别对应
         * 一点不等、等一点时间和等最大时间。在等待队列数据的情况下，该接收队列所在的任务会一直处于阻塞态。
         */
        getTemp = xQueueReceive(queueTempHdlr, &temp, pdMS_TO_TICKS(10));
        getHumi = xQueueReceive(queueHumiHdlr, &humi, pdMS_TO_TICKS(10));
        /* LoRa通信连接字 */
        vLoRaFrameHeaderSend(xLoRaGateConfig.ucLoRaGateIdentifier);
        /* 检测接收队列是否成功 */
        if (getTemp == pdTRUE && getHumi == pdTRUE)
        {
            vLoRaDataSend(xLoRaNode1Config.ucLoRaNode1Identifier, xLoRaSensorID.ucIdDht11, temp, humi, xLoRaExecutorID.ucIdLed, commandLed, xLoRaExecutorID.ucIdFan, commandFan, xLoRaExecutorID.ucIdHumidifier, commandHumidifier);
            vTaskDelay(1000);
        }
        if (xSemaphoreTake(semLedOnHdlr, pdMS_TO_TICKS(10)) == pdTRUE)
        {
            commandLed = LORA_EXECUTOR_COMMAND_ON;
        } else if (xSemaphoreTake(semLedOffHdlr, pdMS_TO_TICKS(10)) == pdTRUE)
        {
            commandLed = LORA_EXECUTOR_COMMAND_OFF;
        } 
        else if (xSemaphoreTake(semFanOnHdlr, pdMS_TO_TICKS(10)) == pdTRUE)
        {
            commandFan = LORA_EXECUTOR_COMMAND_ON;
        } else if (xSemaphoreTake(semFanOffHdlr, pdMS_TO_TICKS(10)) == pdTRUE)
        {
            commandFan = LORA_EXECUTOR_COMMAND_OFF;
        } else if (xSemaphoreTake(semHumidifierOnHdlr, pdMS_TO_TICKS(10)) == pdTRUE)
        {
            commandHumidifier = LORA_EXECUTOR_COMMAND_ON;
        } else if (xSemaphoreTake(semHumidifierOffHdlr, pdMS_TO_TICKS(10)) == pdTRUE)
        {
            commandHumidifier = LORA_EXECUTOR_COMMAND_OFF;
        }
        vTaskDelay(500);
    }
}

/**
  * @brief  LoRa消息接收并处理任务函数
  * @note   从USART3接收LoRa模块传输的执行器控制消息，并根据信号量控制相应的执行器状态。
  * @param  *parameters 任务参数，未使用，传入NULL。
  * @retval None
  */
void taskLoRaReceived(void *parameters)
{
    uint8_t gatewayId = 0, executorId = 0, executorState = 0;
    uint8_t getGatewayId = 0, getGatewayExecutorId = 0, getGatewayExecutorState = 0;
    while (1)
    {
        getGatewayId = xQueueReceive(xQueueUsart3IrqHdlr, &gatewayId, portMAX_DELAY);
        getGatewayExecutorId = xQueueReceive(xQueueUsart3IrqHdlr, &executorId, portMAX_DELAY);
        getGatewayExecutorState = xQueueReceive(xQueueUsart3IrqHdlr, &executorState, portMAX_DELAY);
        while (getGatewayId == pdTRUE && getGatewayExecutorId == pdTRUE && getGatewayExecutorState == pdTRUE)
        {
            if (xLoRaMsgProcess(gatewayId, executorId, executorState) == statusLedOn)
            {
                pc13LedOn();
                if (getPc13LedStatus())
                {
                    xSemaphoreGive(semLedOnHdlr);
                } else
                {
                    vDebugLogPrint("Can't turn on the LED.");
                }
            } else if (xLoRaMsgProcess(gatewayId, executorId, executorState) == statusLedOff)
            {
                pc13LedOff();
                if (!getPc13LedStatus())
                {
                    xSemaphoreGive(semLedOffHdlr);
                } else
                {
                    vDebugLogPrint("Can't turn off the LED.");
                }
            } 
            else if (xLoRaMsgProcess(gatewayId, executorId, executorState) == statusFanOn)
            {
                fanOn();
                if (getPa0RelayStatus())
                {
                    xSemaphoreGive(semFanOnHdlr);
                } else
                {
                    vDebugLogPrint("Can't turn on the fan.");
                }
            } else if (xLoRaMsgProcess(gatewayId, executorId, executorState) == statusFanOff)
            {
                fanOff();
                if (!getPa0RelayStatus())
                {
                    xSemaphoreGive(semFanOffHdlr);
                } else
                {
                    vDebugLogPrint("Can't turn off the fan.");
                }
            } else if (xLoRaMsgProcess(gatewayId, executorId, executorState) == statusHumidifierOn)
            {
                xSemaphoreGive(semHumidifierOnHdlr);
                if (!getPa0RelayStatus())
                {
                    xSemaphoreGive(semHumidifierOnHdlr);
                } else
                {
                    vDebugLogPrint("Can't turn on the humifidier.");
                }
            } else if (xLoRaMsgProcess(gatewayId, executorId, executorState) == statusHumidifierOff)
            {
                if (!getPa0RelayStatus())
                {
                    xSemaphoreGive(semHumidifierOffHdlr);
                } else
                {
                    vDebugLogPrint("Can't turn off the humifidier.");
                }
            }
            break;
        }
    }
}

void taskTrace(void *parameters)
{
    char buffer[50];
    uint8_t temp = 0, humi = 0;
    /* 定义一个返回值判断是否接收成功 */
    BaseType_t getTemp = 0L, getHumi = 0L;
    while (1)
    {
        /* 接收队列：队列句柄、传输转存的目标变量或数组和未接收到数据的等待时长，分3种情况，分别为0、0~portMAX_DELAY和portMAX_DELAY三种，分别对应
         * 一点不等、等一点时间和等最大时间。在等待队列数据的情况下，该接收队列所在的任务会一直处于阻塞态。
         */
        getTemp = xQueueReceive(queueTempHdlr, &temp, pdMS_TO_TICKS(10));
        getHumi = xQueueReceive(queueHumiHdlr, &humi, pdMS_TO_TICKS(10));
        /* 检测接收队列是否成功 */
        if (getTemp == pdTRUE && getHumi == pdTRUE)
        {
            snprintf(buffer, sizeof(buffer), "Temp=%d, Humi=%d\r\n", temp, humi);
            vDebugLogPrint(buffer);
            vTaskDelay(1000);
        }
    }
}

/**
  * @brief  创建任务列表函数
  * @note   创建所有需要的FreeRTOS任务。
  * @param  None
  * @retval None
  */
void createTasksList(void)
{
    /* 创建任务，参数分别为任务函数名称、任务名字、栈大小、返回参数值、优先级、任务句柄。 */
//    xTaskCreate(
//               (TaskFunction_t        ) taskWorkStateLed,
//               (char *                ) "TaskName_StateLed", 
//               (configSTACK_DEPTH_TYPE) 128,
//               (void *                ) NULL, 
//               (UBaseType_t           ) 2,
//               (TaskHandle_t *        ) &taskWorkStateLedHdlr);
    xTaskCreate(
               (TaskFunction_t        ) taskDht11,
               (char *                ) "TaskName_DHT11", 
               (configSTACK_DEPTH_TYPE) 256,
               (void *                ) NULL, 
               (UBaseType_t           ) 2,
               (TaskHandle_t *        ) &taskDht11Hdlr);
    xTaskCreate(
               (TaskFunction_t        ) taskLoRaSend,
               (char *                ) "TaskName_LoRaSendToGateNode", 
               (configSTACK_DEPTH_TYPE) 512,
               (void *                ) NULL, 
               (UBaseType_t           ) 2,
               (TaskHandle_t *        ) &taskLoRaSendHdlr);
    xTaskCreate(
               (TaskFunction_t        ) taskLoRaReceived,
               (char *                ) "TaskName_LoRaReceivedMessage", 
               (configSTACK_DEPTH_TYPE) 512,
               (void *                ) NULL, 
               (UBaseType_t           ) 2,
               (TaskHandle_t *        ) &taskLoRaReceiveHdlr);
    xTaskCreate(
               (TaskFunction_t        ) taskTrace,
               (char *                ) "TaskName_DebugTrace", 
               (configSTACK_DEPTH_TYPE) 512,
               (void *                ) NULL, 
               (UBaseType_t           ) 2,
               (TaskHandle_t *        ) &taskTraceHdlr);
}

/**
  * @brief  创建队列列表函数
  * @note   创建所有需要的FreeRTOS队列。
  * @param  None
  * @retval None
  */
void createQueuesList(void)
{
    /* 创建队列有两个参数，第一个是队列的长度,也就是这个队列能存放多少个数据；
     * 第二个参数就是每个数据的大小，单位为字节，在stm32中uint8_t指针的长度一般是4字节。
     * 原函数的返回值就是队列句柄的结构体，所以需要在上面定义一个变量接收队列句柄。 */
    queueTempHdlr      = xQueueCreate(
                                      (UBaseType_t) 5,
                                      (UBaseType_t) sizeof(uint8_t));
    queueHumiHdlr      = xQueueCreate(
                                      (UBaseType_t) 5,
                                      (UBaseType_t) sizeof(uint8_t));
    xQueueUsart3IrqHdlr = xQueueCreate(
                                      (UBaseType_t) 64,
                                      (UBaseType_t) sizeof(uint8_t));
    configASSERT(xQueueUsart3IrqHdlr != NULL);
}

/**
  * @brief  创建信号量列表函数
  * @note   创建所有需要的FreeRTOS信号量。
  * @param  None
  * @retval None
  */
void createSemaphoresList(void)
{
    semLedOnHdlr  = xSemaphoreCreateBinary();
    semLedOffHdlr = xSemaphoreCreateBinary();
    semFanOnHdlr  = xSemaphoreCreateBinary();
    semFanOffHdlr = xSemaphoreCreateBinary();
}

/**
  * @brief  主函数
  * @note   初始化系统各模块，创建队列、信号量和任务，并启动调度器。
  * @param  None
  * @retval None
  */
int main(void)
{
    fanRelayInit();
    humidifierRelayInit();
    delayInit();
    pc13LedInit();
    pb0LedInit();
    vUsartInit(USART1, 115200);
    vUsartInit(USART3, 115200);
    createQueuesList();
    createSemaphoresList();
    createTasksList();
    vTaskStartScheduler();
    while(1)
    {
    
    }
}
