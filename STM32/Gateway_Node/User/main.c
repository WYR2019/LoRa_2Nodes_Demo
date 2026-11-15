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
TaskHandle_t taskWorkStatusLedHdlr;
TaskHandle_t taskNetworkInitHdlr;
TaskHandle_t taskMqttPublishHdlr;
TaskHandle_t taskMqttSubscribeHdlr;
TaskHandle_t taskLoRaConfigQueryHdlr;
TaskHandle_t taskLoRaRecvFromNodesHdlr;

/* 创建队列句柄 */
QueueHandle_t queueUsart1IrqHdlr;
QueueHandle_t queueUsart2IrqHdlr;
QueueHandle_t queueUsart3IrqHdlr;

QueueHandle_t queueLoRaRecvFromNodesHdlr;

/**
  * @brief  状态指示LED任务
  * @note   通过控制PC13引脚的LED灯，实现系统状态的指示功能。
  * @param  *parameters 任务参数，若没有特定的参数则设置为空指针
  * @retval None
  */
void taskWorkStatusLed(void *parameters)
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
  * @brief  初始化Wi-Fi模块网络任务
  * @note   通过ESP8266模块初始化MQTT协议连接。
  * @param  *parameters 任务参数，若没有特定的参数则设置为空指针
  * @retval None
  */
void taskNetworkInit(void *parameters)
{
    while (1)
    {
        esp8266AtTest();
        if (esp8266NetModeChoose(STA) == pdTRUE)
        {
            trace("Set ESP8266 Net Mode Successfully.\r\n");
        } else
        {
            continue;
        } 
        if (esp8266JoinAp(ESP8266_APSSID, ESP8266_APPWD) == pdTRUE)
        {
            trace("Wifi Join To AP Successfully.\r\n");
        } else
        {
            continue;
        }
        if (esp8266MqttInit(ESP8266_ALIYUN_MQTT_USERNAME, ESP8266_ALIYUN_MQTT_PASSWORD, ESP8266_ALIYUN_MQTT_CLIENTID, 
            ESP8266_ALIYUN_MQTT_IP, ESP8266_ALIYUN_MQTT_PORT, ESP8266_ALIYUN_MQTT_SUBSCRIBE_TOPIC) == pdTRUE)
        {
            trace("Aliyun MQTT Init Successfully.\r\n");
            /* 通知MQTT发布任务 */
            xTaskNotifyGive(taskMqttPublishHdlr);
            xTaskNotifyGive(taskMqttSubscribeHdlr);
            vTaskDelete(NULL);
        } else
        {
            continue;
        }
    }
}

/**
  * @brief  MQTT发布测试任务
  * @note   通过ESP8266模块向MQTT服务器发布测试消息。
  * @param  *parameters 任务参数，若没有特定的参数则设置为空指针
  * @retval None
  */
void taskMqttPublish(void *parameters)
{
    /* 任务通知接收函数 */
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    char msg[128] = {0};
    BaseType_t getTemperature = 0L, getHumidity = 0L;
    uint8_t temperature = 0, humidity = 0;
    while (1)
    {
        getTemperature = xQueueReceive(queueLoRaRecvFromNodesHdlr, &temperature, portMAX_DELAY);
        getHumidity = xQueueReceive(queueLoRaRecvFromNodesHdlr, &humidity, portMAX_DELAY);
        trace("MQTT temperature:%d C, humidity:%d%%\r\n", temperature, humidity);
        vTaskDelay(300);
        snprintf(msg, sizeof(msg), "{\"node1_info\": {\"temperature\":\"%d\" , \"humidity\":\"%d\" },{ \"led\":\"off\" }}", 
                                        temperature, humidity);
        if (mqttPublish(ESP8266_ALIYUN_MQTT_PUBLISH_TOPIC, msg) == pdTRUE)
        {
            trace("MQTT Publish Successfully.\r\n");
        }
    }
}

/**
  * @brief  MQTT订阅任务
  * @note   通过ESP8266模块订阅MQTT主题并处理接收到的命令。
  * @param  *parameters 任务参数，若没有特定的参数则设置为空指针
  * @retval None
  */
void taskMqttSubscribe(void *parameters)
{
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    while (1)
    {
        if (mqttCmdParse(ESP8266_ALIYUN_MQTT_SUBSCRIBE_TOPIC) == MQTT_CMD_LED_ON)
        {
            trace("Lights on\r\n");
        }
        else if (mqttCmdParse(ESP8266_ALIYUN_MQTT_SUBSCRIBE_TOPIC) == MQTT_CMD_LED_OFF)
        {
            trace("Lights off\r\n");
        }
        else if (mqttCmdParse(ESP8266_ALIYUN_MQTT_SUBSCRIBE_TOPIC) == MQTT_CMD_FAN_ON)
        {
            trace("Fan on\r\n");
        }
        else if (mqttCmdParse(ESP8266_ALIYUN_MQTT_SUBSCRIBE_TOPIC) == MQTT_CMD_FAN_OFF)
        {
            trace("Fan off\r\n");
        }
        else if (mqttCmdParse(ESP8266_ALIYUN_MQTT_SUBSCRIBE_TOPIC) == MQTT_CMD_HUMIDIFIER_ON)
        {
            trace("Humidifier on\r\n");
        }
        else if (mqttCmdParse(ESP8266_ALIYUN_MQTT_SUBSCRIBE_TOPIC) == MQTT_CMD_HUMIDIFIER_OFF)
        {
            trace("Humidifier off\r\n");
        }
        vTaskDelay(1000);
    }
}

void taskLoRaRecvFromNodes(void *parameters)
{
    LoRaRecvData_t loRaRecvData;
    uint8_t temperature = 0, humidity = 0;
    bool getNode1OnlineState = false, getNode2OnlineState = false;
    while (1)
    {
        receiveFromNodes(&loRaRecvData);
        vTaskDelay(300);
        getNode1OnlineState = loRaRecvData.NodeOnlineState_t.Node1OnlineState;
        getNode2OnlineState = loRaRecvData.NodeOnlineState_t.Node2OnlineState;
        if (getNode1OnlineState)
        {
            trace("Node1 is online.\r\n");
        } else
        {
            trace("Node1 is offline.\r\n");
        }
        if (getNode2OnlineState)
        {
            trace("Node2 is online.\r\n");
        } else
        {
            trace("Node2 is offline.\r\n");
        }
        temperature = loRaRecvData.SensorData_t.temperature;
        humidity = loRaRecvData.SensorData_t.humidity;
        trace("temperature:%d C, humidity:%d%%\r\n", loRaRecvData.SensorData_t.temperature, loRaRecvData.SensorData_t.humidity);
        switch (loRaRecvData.ExecutorData_t.lightStatus)
        {
            case 0:
                trace("lightStatus:off\r");
                break;
            case 1:
                trace("lightStatus:on\r");
                break;
            default:
                trace("lightStatus:unknown\r");
                break;
        }
        switch (loRaRecvData.ExecutorData_t.fanStatus)
        {
            case 0:
                trace("fanStatus:off\r\n");
                break;
            case 1:
                trace("fanStatus:on\r\n");
                break;
            default:
                trace("fanStatus:unknown\r\n");
                break;
        }
        xQueueSend(queueLoRaRecvFromNodesHdlr, &temperature, portMAX_DELAY);
        xQueueSend(queueLoRaRecvFromNodesHdlr, &humidity, portMAX_DELAY);
    }
}

/** 
  * @brief  创建所有任务列表
  * @note   在此函数中创建所有需要的任务，并为每个任务分配适当的堆栈大小和优先级。
  * @param  None
  * @retval None
  */
void createTasksList(void)
{
    xTaskCreate(
               (TaskFunction_t        ) taskWorkStatusLed,
               (char *                ) "TaskName_StateLed", 
               (configSTACK_DEPTH_TYPE) 128,
               (void *                ) NULL, 
               (UBaseType_t           ) 2,
               (TaskHandle_t *        ) &taskWorkStatusLedHdlr);
    xTaskCreate(
               (TaskFunction_t        ) taskNetworkInit,
               (char *                ) "TaskName_WifiInitMQTTProtocol", 
               (configSTACK_DEPTH_TYPE) 512,
               (void *                ) NULL,
               (UBaseType_t           ) 2,
               (TaskHandle_t *        ) &taskNetworkInitHdlr);
    xTaskCreate(
               (TaskFunction_t        ) taskMqttPublish,
               (char *                ) "TaskName_MqttPublishTest", 
               (configSTACK_DEPTH_TYPE) 512,
               (void *                ) NULL,
               (UBaseType_t           ) 2,
               (TaskHandle_t *        ) &taskMqttPublishHdlr);
    xTaskCreate(
               (TaskFunction_t        ) taskMqttSubscribe,
               (char *                ) "TaskName_MqttSubscribe", 
               (configSTACK_DEPTH_TYPE) 512,
               (void *                ) NULL,
               (UBaseType_t           ) 2,
               (TaskHandle_t *        ) &taskMqttSubscribeHdlr);
    xTaskCreate(
               (TaskFunction_t        ) taskLoRaRecvFromNodes,
               (char *                ) "TaskName_LoRaReceiveFromNodes", 
               (configSTACK_DEPTH_TYPE) 512,
               (void *                ) NULL,
               (UBaseType_t           ) 2,
               (TaskHandle_t *        ) &taskLoRaRecvFromNodesHdlr);
}

/**
  * @brief  创建所有队列列表
  * @note   在此函数中创建所有需要的队列，并为每个队列分配适当的长度和项大小。
  * @param  None
  * @retval None
  */
void createQueuesList(void)
{
#if (ENABLE_FREERTOS_API_QUEUE_USART1_IRQ == 1)
    queueUsart1IrqHdlr = xQueueCreate(
                                      (UBaseType_t) 64,
                                      (UBaseType_t) sizeof(uint8_t *));
#endif
#if (ENABLE_FREERTOS_API_QUEUE_USART2_IRQ == 1)
    queueUsart2IrqHdlr = xQueueCreate(
                                      (UBaseType_t) 1024,
                                      (UBaseType_t) sizeof(uint8_t *));
#endif
#if (ENABLE_FREERTOS_API_QUEUE_USART3_IRQ == 1)
    queueUsart3IrqHdlr = xQueueCreate(
                                      (UBaseType_t) 64,
                                      (UBaseType_t) sizeof(uint8_t *));
#endif
    queueLoRaRecvFromNodesHdlr = xQueueCreate(
                                      (UBaseType_t) 64,
                                      (UBaseType_t) sizeof(uint8_t));
}

/**
  * @brief  创建所有信号量列表
  * @note   在此函数中创建所有需要的信号量。
  * @param  None
  * @retval None
  */
void createSemaphoresList(void)
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
    usartInit(USART1, 115200);
    usartInit(USART2, 115200);
    usartInit(USART3, 115200);
    delayInit();
    pc13LedInit();
    esp8266GpioConfig();
    createTasksList();
    createQueuesList();
    createSemaphoresList();
    vTaskStartScheduler();
}
