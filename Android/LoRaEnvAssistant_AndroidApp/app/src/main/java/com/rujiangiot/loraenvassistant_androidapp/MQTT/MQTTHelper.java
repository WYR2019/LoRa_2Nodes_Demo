package com.rujiangiot.loraenvassistant_androidapp.MQTT;

import android.util.Log;

import com.rujiangiot.loraenvassistant_androidapp.MQTT.Entity.MQTT;
import com.rujiangiot.loraenvassistant_androidapp.MQTT.Interface.OnSubscribedListener;
import com.rujiangiot.loraenvassistant_androidapp.Tools.JSON.JSONHelper;

import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;

import java.util.List;
import java.util.Map;
import java.util.Objects;

/**
 * @ClassName MQTTHelper
 * @Auther name
 * @Date 2025/5/1
 * @Description
 */
public class MQTTHelper {
    public enum connectStatus{
        CONNECT_SUCCESS,
        CONNECT_FAIL
    }

    public final String MQTT_SERVER_URI = "tcp://iot-06z00by9al78fmz.mqtt.iothub.aliyuncs.com:1883";
    public final String MQTT_CLIENT_ID = "k11ilf46LJ0.Android|securemode=2," +
            "signmethod=hmacsha256,timestamp=1776051209143|";
    public final String MQTT_USERNAME = "Android&k11ilf46LJ0";// 不能重名
    public final String MQTT_PASSWORD = "f804d74db7a4e1f5590d7a8cb301374eecbc24851bbc522d80b3fce2af23a346";
    public final String MQTT_SUBSCRIBE_TOPIC = "/k11ilf46LJ0/Android/user/get";// 接收来自云平台的消息
    public final String MQTT_PUBLISH_TOPIC = "/k11ilf46LJ0/Android/user/update";
    private MqttClient mqttClient;

    private OnSubscribedListener dataListener;                                                                // 回调接口

    public void setOnDataReceivedListener(OnSubscribedListener listener) {
        this.dataListener = listener;
    }

    public void mqttGetSubscribeData() {
        MQTT mqtt = new MQTT();
        mqttClient.setCallback(new MqttCallback() {
            @Override
            public void connectionLost(Throwable cause) {
                if (cause.equals("Caused by: java.net.SocketException: Connection reset"))
                    Log.e("MQTT", "连接中断", cause);
            }
            @Override
            public void messageArrived(String topic, MqttMessage message) {
                String msg = new String(message.getPayload());
                Log.v("订阅数据", msg);
                Map<String, String> parsedData = JSONHelper.parseValues(msg,
                        "id", "mode", "timestamp", "temperature", "humidity", "light",
                        "fan", "humidifier", "smoke", "beep", "windows", "smoke_safety",
                        "fire_safety");
                mqtt.mqttDeviceId = parsedData.get("id");
                mqtt.mqttDeviceMode = parsedData.get("mode");
                mqtt.mqttTimestamp = Long.parseLong(Objects.requireNonNull(parsedData.get("timestamp")));
                List<String> keys = JSONHelper.parseKeys(msg, "node1", "node2");                    // 查找是否存在 node1 或 node2
                // keys = ["node2", "node1"]   （深度优先，node2 先出现）
                /* 可以判断是否包含某个键名 */
                mqtt.mqttIsNode1Connected = keys.contains("node1");                                             // true
                mqtt.mqttIsNode2Connected = keys.contains("node2");
                mqtt.mqttTemp = parsedData.get("temperature");
                mqtt.mqttHumi = parsedData.get("humidity");
                mqtt.mqttLight = parsedData.get("light");
                mqtt.mqttFan = parsedData.get("fan");
                mqtt.mqttHumidifier = parsedData.get("humidifier");
                mqtt.mqttSmoke = parsedData.get("smoke");
                mqtt.mqttBeep = parsedData.get("beep");
                mqtt.mqttWindows = parsedData.get("windows");
                mqtt.mqttSmokeSafety = parsedData.get("smoke_safety");
                mqtt.mqttFireSafety = parsedData.get("fire_safety");
                log(mqtt);
                try {
                    org.json.JSONObject obj = new org.json.JSONObject(msg);
                    Log.v("TAG", "顶层 keys = " + obj.keys());  // 只会看到 node1
                } catch (Exception e) { e.printStackTrace(); }
                /* 回调数据 */
                if (dataListener != null) {
                    dataListener.onDeviceConnected(mqtt.mqttDeviceId, mqtt.mqttDeviceMode, mqtt.mqttTimestamp);
                    dataListener.onNodesConnected(mqtt.mqttIsNode1Connected);
                    dataListener.onTemperatureReceived(mqtt.mqttTemp);
                    dataListener.onHumidityReceived(mqtt.mqttHumi);
                }
            }

            @Override
            public void deliveryComplete(IMqttDeliveryToken token) {
                Log.v("MQTT", "消息发送完成");
            }
        });
    }

    public void mqttPublish(String topic, String payload) {
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

    /**
     * MQTT初始化连接函数
     *
     * @return
     */
    public connectStatus mqttInit() {
        try {
            mqttClient = new MqttClient(MQTT_SERVER_URI, MQTT_CLIENT_ID, new MemoryPersistence());  // 实例化mqttclient
            /* 创建并实例化一个mqtt的连接参数对象 */
            MqttConnectOptions mqttConnectOptions = new MqttConnectOptions();
            /* 设置对应的参数 */
            mqttConnectOptions.setUserName(MQTT_USERNAME);
            mqttConnectOptions.setPassword(MQTT_PASSWORD.toCharArray());
            mqttConnectOptions.setConnectionTimeout(60);
            mqttConnectOptions.setKeepAliveInterval(50);
            mqttConnectOptions.setAutomaticReconnect(true);
            mqttConnectOptions.setCleanSession(false);
            /* 连接 */
            mqttClient.connect(mqttConnectOptions);
            /* 等待连接完成 */
            Thread.sleep(1000);                                                                 // 等待1秒，确保连接稳定
            if (mqttClient.isConnected()) {
                mqttClient.subscribe(MQTT_SUBSCRIBE_TOPIC);
                Log.v("MQTT", "连接成功并订阅主题");
                return connectStatus.CONNECT_SUCCESS;
            } else {
                Log.e("MQTT", "连接失败");
                return connectStatus.CONNECT_FAIL;
            }
        } catch (Exception e) {
            Log.e("MQTT", "连接异常", e);
            return connectStatus.CONNECT_FAIL;
        }
    }

    /**
     * 断开连接
     */
    public void mqttDisconnect() {
        try {
            if (mqttClient != null && mqttClient.isConnected()) {
                mqttClient.disconnect();
                Log.v("MQTT", "断开连接");
            }
        } catch (Exception e) {
            Log.e("MQTT", "断开连接异常", e);
        }
    }

    private void log(MQTT mqtt) {
        Log.v("DeviceId", "DeviceId = " + mqtt.mqttDeviceId);
        Log.v("Mode", "Mode = " + mqtt.mqttDeviceMode);
        Log.v("Timestamp", "Timestamp = " + mqtt.mqttTimestamp);
        Log.v("HasNode1", "Node1 is connected = " + mqtt.mqttIsNode1Connected);
        Log.v("HasNode2", "Node2 is connected = " + mqtt.mqttIsNode2Connected);
        Log.v("Temp", "Temperature = " + (mqtt.mqttTemp != null ? mqtt.mqttTemp : "未知"));
        Log.v("Humi", "Humidity = " + (mqtt.mqttHumi != null ? mqtt.mqttHumi : "未知"));
        Log.v("Light", "Light " + mqtt.mqttLight);
        Log.v("Fan", "Fan " + mqtt.mqttFan);
        Log.v("Humidifier", "Humidifier " + mqtt.mqttHumidifier);
        Log.v("Smoke", "Smoke " + mqtt.mqttSmoke);
        Log.v("Beep", "Beep " + mqtt.mqttBeep);
        Log.v("Windows", "Windows " + mqtt.mqttWindows);
        Log.v("SmokeSafety", "SmokeSafety = " + mqtt.mqttSmokeSafety);
        Log.v("FireSafety", "FireSafety = " + mqtt.mqttFireSafety);
    }
}