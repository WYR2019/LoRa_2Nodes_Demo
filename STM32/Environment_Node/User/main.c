#include "stm32f10x.h"                  // Device header
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "LED.h"
#include "DHT11.h"
#include "USART3.h"
#include "LoRa.h"
#include "Relay.h"

/* 在创建任务前，需要创建一个任务句柄，每个任务句柄与任务一一对应。 */
TaskHandle_t xStateLedTaskHdlr;
TaskHandle_t xDht11TaskHdlr;
TaskHandle_t xRelayCtrlTaskHdlr;
TaskHandle_t xLoRaToGateTskHdlr;
TaskHandle_t xLoRaMsgRecTskHdlr;

/* 创建队列句柄 */
QueueHandle_t xQueueTempHdlr;
QueueHandle_t xQueueHumiHdlr;
QueueHandle_t xQueueUsart3ReHdlr;

/* 创建二值信号量句柄 */
SemaphoreHandle_t xSemLedOnHandler;
SemaphoreHandle_t xSemLedOffHandler;

/* 闪烁LED任务函数，参数为空指针，返回NULL。 */
void vStateLedTask(void *pvParameters)
{
    while(1)
    {
        vPc13LedOn();
        vTaskDelay(1000);
        vPc13LedOff();
        vTaskDelay(1000);
    }
}

void vDht11Task(void *pvParameters)
{
    DHT11Data_t xDHT11Data;
    /* 数据存储数组。要求与队列参数设置大小一一对应。 */
    uint8_t ucSendTempData = 0;
    uint8_t ucSendHumiData = 0;
    if (vDht11Init() != 0)
    {
        /* 初始化失败，进行错误处理 */ 
        ucSendTempData = ucSendHumiData = 0xAA;
        xQueueSend(xQueueTempHdlr, &ucSendTempData, pdMS_TO_TICKS(10));
        xQueueSend(xQueueHumiHdlr, &ucSendHumiData, pdMS_TO_TICKS(10));
    } 
    while (1)
    {   
        /* code */
        if (vDht11ReadData(&xDHT11Data.ucTemp,&xDHT11Data.ucHumi) == 0)
        {
            /* code */
            ucSendTempData = xDHT11Data.ucTemp;
            ucSendHumiData = xDHT11Data.ucHumi;
            /* 写队列。第一个参数是队列句柄，第二个是写入队列的地址（如数组），第三个是因队列满造成阻塞时的等待时长，也就是无法写入数据时的等待时长，以操作系统默认时钟节拍为准。 */
            xQueueSend(xQueueTempHdlr, &ucSendTempData, pdMS_TO_TICKS(10));
            xQueueSend(xQueueHumiHdlr, &ucSendHumiData, pdMS_TO_TICKS(10));
        } else
        {
            ucSendTempData = ucSendHumiData = 0xAB;
        }        
    }
}

void vLoRaToGatePktTask(void *pvParameters)
{
    uint8_t ucRecTempData = 0;
    uint8_t ucRecHumiData = 0;
    uint8_t testData = 0xAC;
    uint8_t ledOff   = 0xAD;
    /* 定义一个返回值判断是否接收成功 */
    BaseType_t xQueueTempRetval, xQueueHumiRetval;
    while (1)
    {
        /* code */
        /* 接收队列：队列句柄、传输转存的目标变量或数组和未接收到数据的等待时长，分3种情况，分别为0、0~portMAX_DELAY和portMAX_DELAY三种，分别对应
         * 一点不等、等一点时间和等最大时间。在等待队列数据的情况下，该接收队列所在的任务会一直处于阻塞态。
         */
        xQueueTempRetval = xQueueReceive(xQueueTempHdlr, &ucRecTempData, pdMS_TO_TICKS(10));
        xQueueHumiRetval = xQueueReceive(xQueueHumiHdlr, &ucRecHumiData, pdMS_TO_TICKS(10));
        /* 发送 */
        vLoRaConnectionPkt(xLoRaGateConfig.ucLoRaGateChannel);
        vLoRaToGateIdPkt(xLoRaNode1Config.ucLoRaNode1Identifier);
        vLoRaToGateSenIdPkt(xLoRaSensorID.ucIdDht11);
        /* 检测接收队列是否成功 */
        if (xQueueTempRetval == pdTRUE && xQueueHumiRetval == pdTRUE)
        {
            /* code */
            vUsart3SendArray(&ucRecTempData, 1);
            vUsart3SendArray(&ucRecHumiData, 1);
            
        }
        if (xSemaphoreTake(xSemLedOnHandler, pdMS_TO_TICKS(10)) == pdTRUE)
        {
            /* code */
            vUsart3SendArray(&testData, 1);
            vPc13LedOn();
        } 
        else if (xSemaphoreTake(xSemLedOffHandler, pdMS_TO_TICKS(10)) == pdTRUE)
        {
            /* code */
            vUsart3SendArray(&ledOff, 1);
            vPc13LedOff();
        }
               
        vTaskDelay(1000);
    }
}

void vLoRaMsgRecTask(void *pvParameters)
{
    uint8_t ucDataRecNodeId = 0, ucDataRecExeId = 0, ucDataRecExeSta = 0;
    uint8_t ucRetvalQueueNodeId = 0, ucRetvalQueueExeId = 0, ucRetvalQueueExeSta = 0;
    while (1)
    {
        ucRetvalQueueNodeId = xQueueReceive(xQueueUsart3ReHdlr, &ucDataRecNodeId, portMAX_DELAY);
        ucRetvalQueueExeId = xQueueReceive(xQueueUsart3ReHdlr, &ucDataRecExeId, portMAX_DELAY);
        ucRetvalQueueExeSta = xQueueReceive(xQueueUsart3ReHdlr, &ucDataRecExeSta, portMAX_DELAY);
        while (ucRetvalQueueNodeId == pdTRUE && ucRetvalQueueExeId == pdTRUE && ucRetvalQueueExeSta == pdTRUE)
        {
            /* code */
            if (xLoRaMsgProcess(ucDataRecNodeId, ucDataRecExeId, ucDataRecExeSta) == statusLedOn)
            {
                /* code */
                xSemaphoreGive(xSemLedOnHandler);
            } else if (xLoRaMsgProcess(ucDataRecNodeId, ucDataRecExeId, ucDataRecExeSta) == statusLedOff)
            {
                /* code */
                xSemaphoreGive(xSemLedOffHandler);
            }
            break;
        }
    }
}

void vCreateTasksList(void)
{
    /* 进入临界区 */
    // taskENTER_CRITICAL();
    /* 创建任务，参数分别为任务函数名称、任务名字、栈大小、返回参数值、优先级、任务句柄。 */
//    xTaskCreate(
//               (TaskFunction_t        ) vStateLedTask,
//               (char *                ) "TaskName_StateLed", 
//               (configSTACK_DEPTH_TYPE) 512,
//               (void *                ) NULL, 
//               (UBaseType_t           ) 2,
//               (TaskHandle_t *        ) &xStateLedTaskHdlr);
    xTaskCreate(
               (TaskFunction_t        ) vDht11Task,
               (char *                ) "TaskName_DHT11", 
               (configSTACK_DEPTH_TYPE) 512,
               (void *                ) NULL, 
               (UBaseType_t           ) 2,
               (TaskHandle_t *        ) &xDht11TaskHdlr);
    xTaskCreate(
               (TaskFunction_t        ) vLoRaToGatePktTask,
               (char *                ) "TaskName_LoRaSendToGateway", 
               (configSTACK_DEPTH_TYPE) 512,
               (void *                ) NULL, 
               (UBaseType_t           ) 2,
               (TaskHandle_t *        ) &xLoRaToGateTskHdlr);
    xTaskCreate(
               (TaskFunction_t        ) vLoRaMsgRecTask,
               (char *                ) "TaskName_LoRaReceivedMessage", 
               (configSTACK_DEPTH_TYPE) 512,
               (void *                ) NULL, 
               (UBaseType_t           ) 2,
               (TaskHandle_t *        ) &xLoRaMsgRecTskHdlr);
    /* 退出临界区 */
    // taskEXIT_CRITICAL();
}

void vCreateQueuesList(void)
{
    /* 创建队列有两个参数，第一个是队列的长度,也就是这个队列能存放多少个数据；
     * 第二个参数就是每个数据的大小，单位为字节，在stm32中uint8_t指针的长度一般是4字节。
     * 原函数的返回值就是队列句柄的结构体，所以需要在上面定义一个变量接收队列句柄。 */
    xQueueTempHdlr     = xQueueCreate(
                                     (UBaseType_t) 5,
                                     (UBaseType_t) sizeof(uint8_t));
    xQueueHumiHdlr     = xQueueCreate(
                                     (UBaseType_t) 5,
                                     (UBaseType_t) sizeof(uint8_t));
    xQueueUsart3ReHdlr = xQueueCreate(
                                     (UBaseType_t) 64,
                                     (UBaseType_t) sizeof(uint8_t *));
    if (xQueueTempHdlr == NULL || xQueueHumiHdlr == NULL || xQueueUsart3ReHdlr == NULL)
    {
        /* code */
        vUsart3Printf("Queue Init Failed.\r\n");
    }
}

void vCreateSemaphoreList(void)
{
    xSemLedOnHandler = xSemaphoreCreateBinary();
    xSemLedOffHandler = xSemaphoreCreateBinary();
}

int main(void)
{
    vFanRelayInit();
    vDelayInit();
    vPc13LedInit();
    vUsart3Init(115200);
    vCreateQueuesList();
    vCreateSemaphoreList();
    vCreateTasksList();
    vTaskStartScheduler();
    while(1)
    {
    
    }
}
