#include "stm32f10x.h"                  // Device header
#include "BSP_Delay.h"
#include "BSP_LED.h"
#include "BSP_USART.h"
#include "ESP8266.h"
#include "LoRa.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "App_MQTT.h"

/* 创建一个新任务，需要创建任务句柄,任务句柄与任务函数一一对应。 */
TaskHandle_t xTaskWorkStatusLedHdlr;
TaskHandle_t xTaskNetworkInitHdlr;
TaskHandle_t xTaskMqttPublishHdlr;
TaskHandle_t xTaskMqttSubscribeHdlr;
TaskHandle_t xTaskLoRaConfigQueryHdlr;
TaskHandle_t xTaskGetDht11DataHdlr;

/* 创建队列句柄 */
QueueHandle_t xQueueUsart1IrqHdlr;
QueueHandle_t xQueueUsart2IrqHdlr;
QueueHandle_t xQueueUsart3IrqHdlr;

extern LoRaMsgRecvData_t xLoRaMsgRecvData;

/**
  * @brief  状态指示LED任务
  * @note   通过控制PC13引脚的LED灯，实现系统状态的指示功能。
  * @param  *pvParameters 任务参数，若没有特定的参数则设置为空指针
  * @retval None
  */
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
void vTaskNetworkInit(void *pvParameters)
{
    while (1)
    {
        vEsp8266AtTest();
        if (bEsp8266NetModeChoose(STA) == pdTRUE)
        {
            vDebugLogPrint("Set ESP8266 Net Mode Successfully.\r\n");
        } else
        {
            // vDebugLogPrint("Set ESP8266 Net Mode Failed.\r\n");
            continue;
        } 
        if (bEsp8266JoinAp(ESP8266_APSSID, ESP8266_APPWD) == pdTRUE)
        {
            vDebugLogPrint("Wifi Join To AP Successfully.\r\n");
        } else
        {
            // vDebugLogPrint("Wifi Join To AP Failed.\r\n");
            continue;
        }
        if (bEsp8266MqttInit(ESP8266_ALIYUN_MQTT_USERNAME, ESP8266_ALIYUN_MQTT_PASSWORD, ESP8266_ALIYUN_MQTT_CLIENTID, 
            ESP8266_ALIYUN_MQTT_IP, ESP8266_ALIYUN_MQTT_PORT, ESP8266_ALIYUN_MQTT_SUBSCRIBE_TOPIC) == pdTRUE)
        {
            vDebugLogPrint("Aliyun MQTT Init Successfully.\r\n");
            /* 通知MQTT发布任务 */
            xTaskNotifyGive(xTaskMqttPublishHdlr);
            xTaskNotifyGive(xTaskMqttSubscribeHdlr);
            vTaskDelete(NULL);
        } else
        {
            // vDebugLogPrint("Aliyun MQTT Init Failed.\r\n");
            continue;
        }
    }
}

/**
  * @brief  MQTT发布测试任务
  * @note   通过ESP8266模块向MQTT服务器发布测试消息。
  * @param  *pvParameters 任务参数，若没有特定的参数则设置为空指针
  * @retval None
  */
void vTaskMqttPublish(void *pvParameters)
{
    /* 任务通知接收函数 */
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    char cMsg[128] = {0};
    while (1)
    {
        vLoRaParseDataTest();
        vTaskDelay(500);
        snprintf(cMsg, sizeof(cMsg), "{\"node1\": [{ \"temperature\":\"%d\" , \"humidity\":\"%d\" },{ \"led\":\"off\" }]}", 
                                        xLoRaMsgRecvData.Sensor_t.ucTemperature, xLoRaMsgRecvData.Sensor_t.ucHumidity);
        vDebugLogPrint("Node1 Sensor:temperature:%d⁰C, humidity:%d%\r\n", xLoRaMsgRecvData.Sensor_t.ucTemperature, xLoRaMsgRecvData.Sensor_t.ucHumidity);
        if (bMqttPublish(ESP8266_ALIYUN_MQTT_PUBLISH_TOPIC, cMsg) == pdTRUE)
        {
            vDebugLogPrint("MQTT Publish Successfully.\r\n");
        }
//        vDebugLogPrint("MQTT Publish Failed.\r\n");
    }
}

/**
  * @brief  MQTT订阅任务
  * @note   通过ESP8266模块订阅MQTT主题并处理接收到的命令。
  * @param  *pvParameters 任务参数，若没有特定的参数则设置为空指针
  * @retval None
  */
void vTaskMqttSubscribe(void *pvParameters)
{
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    while (1)
    {
        if (xMqttCmdParse(ESP8266_ALIYUN_MQTT_SUBSCRIBE_TOPIC) == MQTT_CMD_LED_ON)
        {
            vDebugLogPrint("Lights on\r\n");
        }
        else if (xMqttCmdParse(ESP8266_ALIYUN_MQTT_SUBSCRIBE_TOPIC) == MQTT_CMD_LED_OFF)
        {
            vDebugLogPrint("Lights off\r\n");
        }
        else if (xMqttCmdParse(ESP8266_ALIYUN_MQTT_SUBSCRIBE_TOPIC) == MQTT_CMD_FAN_ON)
        {
            vDebugLogPrint("Fan on\r\n");
        }
        else if (xMqttCmdParse(ESP8266_ALIYUN_MQTT_SUBSCRIBE_TOPIC) == MQTT_CMD_FAN_OFF)
        {
            vDebugLogPrint("Fan off\r\n");
        }
        else if (xMqttCmdParse(ESP8266_ALIYUN_MQTT_SUBSCRIBE_TOPIC) == MQTT_CMD_HUMIDIFIER_ON)
        {
            vDebugLogPrint("Humidifier on\r\n");
        }
        else if (xMqttCmdParse(ESP8266_ALIYUN_MQTT_SUBSCRIBE_TOPIC) == MQTT_CMD_HUMIDIFIER_OFF)
        {
            vDebugLogPrint("Humidifier off\r\n");
        }
        vTaskDelay(1000);
    }
}

void vTaskLoRaConfigQuery(void *pvParameters)
{
    while (1)
    {
        for (size_t i = 0; i < 3; i++)
        {
            vLoRaConfigQuery();
            vTaskDelay(500);
        }
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
    // xTaskCreate(
    //            (TaskFunction_t        ) vTaskWorkStatusLed,
    //            (char *                ) "TaskName_StateLed", 
    //            (configSTACK_DEPTH_TYPE) 128,
    //            (void *                ) NULL, 
    //            (UBaseType_t           ) 2,
    //            (TaskHandle_t *        ) &xTaskWorkStatusLedHdlr);
    xTaskCreate(
               (TaskFunction_t        ) vTaskNetworkInit,
               (char *                ) "TaskName_WifiInitMQTTProtocol", 
               (configSTACK_DEPTH_TYPE) 512,
               (void *                ) NULL,
               (UBaseType_t           ) 2,
               (TaskHandle_t *        ) &xTaskNetworkInitHdlr);
    xTaskCreate(
               (TaskFunction_t        ) vTaskMqttPublish,
               (char *                ) "TaskName_MqttPublishTest", 
               (configSTACK_DEPTH_TYPE) 512,
               (void *                ) NULL,
               (UBaseType_t           ) 2,
               (TaskHandle_t *        ) &xTaskMqttPublishHdlr);
    xTaskCreate(
               (TaskFunction_t        ) vTaskMqttSubscribe,
               (char *                ) "TaskName_MqttSubscribe", 
               (configSTACK_DEPTH_TYPE) 512,
               (void *                ) NULL,
               (UBaseType_t           ) 2,
               (TaskHandle_t *        ) &xTaskMqttSubscribeHdlr);
//    xTaskCreate(
//               (TaskFunction_t        ) vTaskLoRaConfigQuery,
//               (char *                ) "TaskName_LoRaQueryTest", 
//               (configSTACK_DEPTH_TYPE) 128,
//               (void *                ) NULL,
//               (UBaseType_t           ) 2,
//               (TaskHandle_t *        ) &xTaskLoRaConfigQueryHdlr);
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
    vEsp8266GpioConfig();
    vCreateTasksList();
    vCreateQueuesList();
    vCreateSemaphoresList();
    vTaskStartScheduler();
}
