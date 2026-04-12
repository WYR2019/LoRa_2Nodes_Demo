package com.example.emqx_smartbadmintongym.MQTT;

import android.util.Log;
import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;
import org.json.JSONException;
import org.json.JSONObject;

/**
 * @ClassName MQTTHelper
 * @Auther name
 * @Date 2025/5/1
 * @Description
 */
public class MQTTHelper {
    public final String serverUri = "tcp://121.36.104.9:1883";
    public final String clientId = "LoRaEnvAssistant&Android";
    public final String userName = "Android";//不能重名
    public final String passWord = "IPSK25r.";
    public final String mqttSubscribeTopic = "/LoRaEnvAssistant/LoRaEnvAssistant&stm32/topics";//接收来自云平台的消息
    public final String mqttPublishTopic = "/LoRaEnvAssistant/LoRaEnvAssistant&Android/topics";
    public final int CONNECT_SUCCESS_IDLE = 1;
    public final int CONNECT_FAIL_IDLE = 2;
    public String jsonTemp, jsonHumi, jsonSmoke, jsonFire, jsonBeep, jsonServo, jsonLight;
    private MqttClient mqttClient;

    // 回调接口
    public interface OnDataReceivedListener {
        void onTemperatureReceived(String temperature);
        void onHumidityReceived(String humidity);
    }

    private OnDataReceivedListener dataListener;

    public void setOnDataReceivedListener(OnDataReceivedListener listener) {
        this.dataListener = listener;
    }

    public void MQTT_Node1_Subscribe() {
        mqttClient.setCallback(new MqttCallback() {
            @Override
            public void connectionLost(Throwable cause) {
                Log.e("MQTT", "连接丢失", cause);
            }

            @Override
            public void messageArrived(String topic, MqttMessage message) throws Exception {
                String msg = new String(message.getPayload());
                Log.v("订阅数据", msg);
                try {
                    JSONObject jsonObject = new JSONObject(msg);
                    jsonTemp = jsonObject.optString("temperature", "--");
                    jsonHumi = jsonObject.optString("humidity", "--");
                    Log.v("温度", jsonTemp);
                    Log.v("湿度", jsonHumi);

                    // 回调数据
                    if (dataListener != null) {
                        dataListener.onTemperatureReceived(jsonTemp);
                        dataListener.onHumidityReceived(jsonHumi);
                    }
                } catch (JSONException e) {
                    Log.e("MQTT", "JSON解析错误", e);
                }
            }

            @Override
            public void deliveryComplete(IMqttDeliveryToken token) {
                Log.v("MQTT", "消息发送完成");
            }
        });
    }

    public void MQTT_Publish(String topic, String payload) {
        if (mqttClient == null || !mqttClient.isConnected()) {
            Log.e("MQTT", "发布失败，MQTT客户端未连接");
            return;
        }
        MqttMessage message = new MqttMessage();
        message.setPayload(payload.getBytes());
        message.setQos(1);
        try {
            mqttClient.publish(topic, message);
            Log.v("MQTT", "发布消息: " + payload);
        } catch (Exception e) {
            Log.e("MQTT", "发布消息异常", e);
        }
    }

    public int MQTT_Init() {
        try {
            // 实例化mqttclient
            mqttClient = new MqttClient(serverUri, clientId, new MemoryPersistence());
            // 创建并实例化一个mqtt的连接参数对象
            MqttConnectOptions mqttConnectOptions = new MqttConnectOptions();
            // 设置对应的参数
            mqttConnectOptions.setUserName(userName);
            mqttConnectOptions.setPassword(passWord.toCharArray());
            mqttConnectOptions.setConnectionTimeout(60);
            mqttConnectOptions.setKeepAliveInterval(50);
            mqttConnectOptions.setAutomaticReconnect(true);
            mqttConnectOptions.setCleanSession(false);
            // 连接
            mqttClient.connect(mqttConnectOptions);
            // 等待连接完成
            Thread.sleep(1000); // 等待1秒，确保连接稳定
            if (mqttClient.isConnected()) {
                mqttClient.subscribe(mqttSubscribeTopic);
                Log.v("MQTT", "连接成功并订阅主题");
                return CONNECT_SUCCESS_IDLE;
            } else {
                Log.e("MQTT", "连接失败");
                return CONNECT_FAIL_IDLE;
            }
        } catch (Exception e) {
            Log.e("MQTT", "连接异常", e);
            return CONNECT_FAIL_IDLE;
        }
    }

    // 断开连接
    public void disconnect() {
        try {
            if (mqttClient != null && mqttClient.isConnected()) {
                mqttClient.disconnect();
                Log.v("MQTT", "断开连接");
            }
        } catch (Exception e) {
            Log.e("MQTT", "断开连接异常", e);
        }
    }
}