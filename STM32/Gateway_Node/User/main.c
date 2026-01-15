#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "USART.h"
#include "LED.h"
#include "ESP8266.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* 创建一个新任务，需要创建任务句柄,任务句柄与任务函数一一对应。 */
TaskHandle_t xTaskWorkStatusLedHdlr;
TaskHandle_t xTaskWifiJoinApHdlr;
TaskHandle_t xTaskWifiNwkInitHdlr;
TaskHandle_t xTaskMqttPublishTestHdlr;

/* 创建队列句柄 */
QueueHandle_t xQueueUsart1IrqHdlr;
QueueHandle_t xQueueUsart2IrqHdlr;
QueueHandle_t xQueueUsart3IrqHdlr;

/**
  * @brief  状态指示LED任务         
  * @note   通过控制PC13引脚的LED灯，实现系统状态的指示功能。
  * @param  *pvParameters 任务参数，若没有特定的参数则设置为空指针
  * @retval None
  */
/* 创建任务 */
void vTaskWorkStatusLed(void *pvParameters)
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
  * @brief  初始化Wi-Fi模块网络任务         
  * @note   通过ESP8266模块初始化MQTT协议连接。
  * @param  *pvParameters 任务参数，若没有特定的参数则设置为空指针
  * @retval None
  */
void vTaskWifiNetWorkInit(void *pvParameters)
{
    while (1)
    {
        vEsp8266GpioConfig();
        vEsp8266AtTest();
        if (bEsp8266NetModeChoose(STA) == pdTRUE)
        {
            vUsartSendString(USART1, "Set ESP8266 Net Mode Successfully.\r\n");
        } else
        {
            /* code */
            vUsartSendString(USART1, "Set ESP8266 Net Mode Failed.\r\n");
            continue;
        } 
        if (bEsp8266JoinAp(ESP8266_APSSID, ESP8266_APPWD) == pdTRUE)
        {
            vUsartSendString(USART1, "Wifi Join To AP Successfully.\r\n");
        } else
        {
            /* code */
            vUsartSendString(USART1, "Wifi Join To AP Failed.\r\n");
            continue;
        }
        #if (ESP8266_MQTT_SERVER_MODE == ALIYUN)
            if (bEsp8266MqttInit(ESP8266_ALIYUN_MQTT_USERNAME, ESP8266_ALIYUN_MQTT_PASSWORD, ESP8266_ALIYUN_MQTT_CLIENTID, 
                ESP8266_ALIYUN_MQTT_IP, ESP8266_ALIYUN_MQTT_PORT, ESP8266_ALIYUN_MQTT_SUBSCRIBE_TOPIC) == pdTRUE)
            {
                /* code */
                vUsartSendString(USART1, "Aliyun MQTT Init Successfully.\r\n");
                /* 通知MQTT发布任务 */
                xTaskNotifyGive(xTaskMqttPublishTestHdlr);
                vTaskDelete(NULL);
            } else
            {
                /* code */
                vUsartSendString(USART1, "Aliyun MQTT Init Failed.\r\n");
                continue;
            }
        #elif (ESP8266_MQTT_SERVER_MODE == EMQX)
            if (bEsp8266MqttInit(ESP8266_EMQX_MQTT_USERNAME, ESP8266_EMQX_MQTT_PASSWORD, ESP8266_EMQX_MQTT_CLIENTID, 
                ESP8266_EMQX_MQTT_SERVER_IP, ESP8266_EMQX_MQTT_SERVER_PORT, ESP8266_EMQX_MQTT_SUBSCRIBE_TOPIC) == pdTRUE)
            {
                /* code */
                vUsartPrintf(USART1, "EMQX MQTT Init Successfully.\r\n");
                xTaskNotifyGive(xTaskMqttPublishTestHdlr);
                vTaskDelete(NULL);
            } else
            {
                /* code */
                vUsartSendString(USART1, "EMQX MQTT Init Failed.\r\n");
                continue;
            }
        #endif
    }
}

/**
  * @brief  MQTT发布测试任务         
  * @note   通过ESP8266模块向MQTT服务器发布测试消息。
  * @param  *pvParameters 任务参数，若没有特定的参数则设置为空指针
  * @retval None
  */
void vTaskMqttPublishTest(void *pvParameters)
{
    /* 任务通知接收函数 */
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    char cCmd[256] = {0};
    while (1)
    {
        /* code */
        #if (ESP8266_MQTT_SERVER_MODE == ALIYUN)
            const char* pcMsg = "test";
            sprintf(cCmd, "AT+MQTTPUB=0,\"%s\",\"%s\",1,0", 
                ESP8266_ALIYUN_MQTT_PUBLISH_TOPIC, pcMsg);
            if (bEsp8266Command(cCmd, "OK", NULL, 3000) == pdTRUE) 
            {
                vUsartSendString(USART1, "Aliyun MQTT Publish Successfully.\r\n");
            } else
            {
                /* code */
                vUsartSendString(USART1, "Aliyun MQTT Publish Failed.\r\n");
            }
            vTaskDelay(500);
        #elif (ESP8266_MQTT_SERVER_MODE == EMQX)
            const char* pcMsg = "test";
            sprintf(cCmd, "AT+MQTTPUB=0,\"%s\",\"%s\",0,0", ESP8266_EMQX_MQTT_PUBLISH_TOPIC, pcMsg);
            if (bEsp8266Command(cCmd, "OK", NULL, 3000) == pdTRUE) 
            {
                vUsartSendString(USART1, "EMQX MQTT Publish Successfully.\r\n");
            } else
            {
                /* code */
                vUsartSendString(USART1, "EMQX MQTT Publish Failed.\r\n");
            }
            vTaskDelay(500);
        #endif
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
               (TaskFunction_t        ) vTaskWorkStatusLed,
               (char *                ) "TaskName_StateLed", 
               (configSTACK_DEPTH_TYPE) 256,
               (void *                ) NULL, 
               (UBaseType_t           ) 2,
               (TaskHandle_t *        ) &xTaskWorkStatusLedHdlr);
    xTaskCreate(
               (TaskFunction_t        ) vTaskWifiNetWorkInit,
               (char *                ) "TaskName_WifiInitMQTTProtocol", 
               (configSTACK_DEPTH_TYPE) 512,
               (void *                ) NULL,
               (UBaseType_t           ) 2,
               (TaskHandle_t *        ) &xTaskWifiNwkInitHdlr);
    xTaskCreate(
               (TaskFunction_t        ) vTaskMqttPublishTest,
               (char *                ) "TaskName_MqttPublishTest", 
               (configSTACK_DEPTH_TYPE) 512,
               (void *                ) NULL,
               (UBaseType_t           ) 2,
               (TaskHandle_t *        ) &xTaskMqttPublishTestHdlr);
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
}

/**
  * @brief  主函数         
  * @note   初始化系统各个模块，创建任务、队列和信号量，并启动调度器。
  * @param  None
  * @retval int
  */
int main(void)
{
    vUsartInit(USART1, 115200);
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
