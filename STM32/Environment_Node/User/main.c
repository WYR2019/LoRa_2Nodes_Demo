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
TaskHandle_t xLoRaRecMsgTskHdlr;
QueueHandle_t xQueueUsart3ReHdlr;

/* 创建队列句柄 */
QueueHandle_t xQueueTempHdlr;
QueueHandle_t xQueueHumiHdlr;

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
    uint8_t ucSendTempBuffer[5] = {0};
    uint8_t ucSendHumiBuffer[5] = {0};
    while (1)
    {   
        /* code */
        if (vDht11Init() != 0)
        {
            /* 初始化失败，进行错误处理 */ 
            // vUsart3Printf("DHT11 Init Failed\r\n.");
            for (int iIndex = 0; iIndex < 5; iIndex++) 
            {
                ucSendTempBuffer[iIndex] = 0xAA;
                vTaskDelay(50);
                ucSendHumiBuffer[iIndex] = 0xAA;
                vTaskDelay(50);
            }
        } 
        if (vDht11ReadData(&xDHT11Data.ucTemp,&xDHT11Data.ucHumi) == 0)
        {
            /* code */
            for (int iIndex = 0; iIndex < 5; iIndex++)
            {
                /* code */
                ucSendTempBuffer[iIndex] = xDHT11Data.ucTemp;
                vTaskDelay(50);
                ucSendHumiBuffer[iIndex] = xDHT11Data.ucHumi;
                vTaskDelay(50);
            }
            /* 写队列。第一个参数是队列句柄，第二个是写入队列的地址（如数组），第三个是因队列满造成阻塞时的等待时长，也就是无法写入数据时的等待时长，以操作系统默认时钟节拍为准。 */
            xQueueSend(xQueueTempHdlr, ucSendTempBuffer, 0);
            xQueueSend(xQueueHumiHdlr, ucSendHumiBuffer, 0);
        } else
        {
            // vUsart3Printf("DHT11 Read Data Failed\r\n.");
            for (int iIndex = 0; iIndex < 5; iIndex++) 
            {
                ucSendTempBuffer[iIndex] = 0xAB;
                vTaskDelay(50);
                ucSendHumiBuffer[iIndex] = 0xAB;
                vTaskDelay(50);
            }
        }        
    }
}

void vLoRaRecMsgTask(void *pvParameters)
{
    uint8_t pucRecIsrBuffer[128] = {0};
    while (1)
    {
        /* code */
        if (xQueueReceive(xQueueUsart3ReHdlr, pucRecIsrBuffer, 0) == pdTRUE)
        {
            /* code */
            vUsart3SendArray(pucRecIsrBuffer, 1);
        }
    }
}

void vLoRaToGatePktTask(void *pvParameters)
{
    uint8_t ucRecTempBuffer[5] = {0};
    uint8_t ucRecHumiBuffer[5] = {0};
    /* 定义一个返回值判断是否接收成功 */
    BaseType_t xQueueTempRetval, xQueueHumiRetval;
    while (1)
    {
        /* code */
        /* 接收队列：队列句柄、给哪个接收数据缓冲区和未接收到数据的等待时长，分3种情况，分别为0、0~portMAX_DELAY和portMAX_DELAY三种，分别对应
         * 一点不等、等一点时间和等最大时间。在等待队列数据的情况下，该接收队列所在的任务会一直处于阻塞态。
         */
        xQueueTempRetval = xQueueReceive(xQueueTempHdlr, ucRecTempBuffer, 0);
        xQueueHumiRetval = xQueueReceive(xQueueHumiHdlr, ucRecHumiBuffer, 0);
        /* 发送 */
        vLoRaConnectionPkt();
        vLoRaToGateIdPkt(xLoRaNode1Config.ucLoRaNode1Identifier);
        vLoRaToGateSenIdPkt(xLoRaSensorID.ucIdDht11);
        /* 检测接收队列是否成功 */
        if (xQueueTempRetval == pdTRUE && xQueueHumiRetval == pdTRUE)
        {
            /* code */
            vUsart3SendArray(&ucRecTempBuffer[1], 1);
            vUsart3SendArray(&ucRecHumiBuffer[1], 1);
        }
        vTaskDelay(1500);
    }
}

void vTasksList(void)
{
    /* 进入临界区 */
    taskENTER_CRITICAL();
    /* 创建任务，参数分别为任务函数名称、任务名字、栈大小、返回参数值、优先级、任务句柄。 */
    xTaskCreate(
               (TaskFunction_t        ) vStateLedTask,
               (char *                ) "TaskName_StateLed", 
               (configSTACK_DEPTH_TYPE) 512,
               (void *                ) NULL, 
               (UBaseType_t           ) 2,
               (TaskHandle_t *        ) &xStateLedTaskHdlr);
    xTaskCreate(
               (TaskFunction_t        ) vDht11Task,
               (char *                ) "TaskName_DHT11", 
               (configSTACK_DEPTH_TYPE) 1024,
               (void *                ) NULL, 
               (UBaseType_t           ) 2,
               (TaskHandle_t *        ) &xDht11TaskHdlr);
    xTaskCreate(
               (TaskFunction_t        ) vLoRaToGatePktTask,
               (char *                ) "TaskName_LoRaSendToGateway", 
               (configSTACK_DEPTH_TYPE) 1024,
               (void *                ) NULL, 
               (UBaseType_t           ) 2,
               (TaskHandle_t *        ) &xLoRaToGateTskHdlr);
    xTaskCreate(
               (TaskFunction_t        ) vLoRaRecMsgTask,
               (char *                ) "TaskName_LoRaReceivedMessage", 
               (configSTACK_DEPTH_TYPE) 1024,
               (void *                ) NULL, 
               (UBaseType_t           ) 2,
               (TaskHandle_t *        ) &xLoRaRecMsgTskHdlr);
    
    /* 退出临界区 */
    taskEXIT_CRITICAL();
}

void vMainQueuesList(void)
{
    /* 创建队列有两个参数，第一个是队列的长度,也就是这个队列能存放多少个数据；
     * 第二个参数就是每个数据的大小，单位为字节，在stm32中uint8_t指针的长度一般是4字节。
     * 原函数的返回值就是队列句柄的结构体，所以需要在上面定义一个变量接收队列句柄。 */
    xQueueTempHdlr     = xQueueCreate(
                                     (UBaseType_t) 5,
                                     (UBaseType_t) sizeof(uint8_t *));
    xQueueHumiHdlr     = xQueueCreate(
                                     (UBaseType_t) 5,
                                     (UBaseType_t) sizeof(uint8_t *));
    xQueueUsart3ReHdlr = xQueueCreate(
                                     (UBaseType_t) 128,
                                     (UBaseType_t) sizeof(uint8_t *));
    if (xQueueTempHdlr == NULL || xQueueHumiHdlr == NULL || xQueueUsart3ReHdlr == NULL)
    {
        /* code */
        vUsart3Printf("Queue Init Failed.\r\n");
    }
}

int main(void)
{
    vFanRelayInit();
    vDelayInit();
    vPc13LedInit();
    vUsart3Init(115200);
    vMainQueuesList();
    vTasksList();
    vTaskStartScheduler();
    while(1)
    {
    
    }
}
