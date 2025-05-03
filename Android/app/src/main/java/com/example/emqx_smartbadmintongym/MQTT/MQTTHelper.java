package com.example.emqx_smartbadmintongym.MQTT;

import android.graphics.Color;
import android.widget.Toast;

import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;

/**
 * @ClassName MQTTHelper
 * @Auther name
 * @Date 2025/5/1
 * @Description
 */
public class MQTTHelper {
    private final String serverUri = "tcp://36.111.167.90:1883";
    private final String clientId = "EMQXSmartBadmintonGym&Android";
    private final String userName = "Android";//不能重名
    private final String passWord = "IPSK25em.";
    private final String mqttSubscribeTopic = "EMQXSmartBadmintonGym/devices/stm32/topics";//接收来自云平台的消息
    private final String mqttPublishTopic = "EMQXSmartBadmintonGym/devices/Android/topics";

    private MqttClient mqttClient;
    private MqttConnectOptions mqttConnectOptions;

    public void MQTT_Init(){
        try {
            //实例化mqttclient,填入我们定义的serverURI和clientID,然后memorypersistence设置clientid的保存形式，默认以内存保存
            mqttClient = new MqttClient(serverUri,clientId, new MemoryPersistence());
            //创建并实例化一个mqtt的连接参数对象
            mqttConnectOptions = new MqttConnectOptions();
            //设置对应的参数
            mqttConnectOptions.setUserName(userName);
            mqttConnectOptions.setPassword(passWord.toCharArray());
            mqttConnectOptions.setConnectionTimeout(60);//设置超时时间30s
            mqttConnectOptions.setKeepAliveInterval(50);//设置心跳时间50s
            //设置是否清空session,设置为false表示服务器会保留客户端的连接记录，设置为true时表示每次连接到服务器都以新的身份连接
            mqttConnectOptions.setAutomaticReconnect(true);
            mqttConnectOptions.setCleanSession(false);
            //初始化成功后开始连接
            MQTT_Connect();
        }catch (Exception e){
            e.printStackTrace();
        }
    }
    public void MQTT_Connect(){
        try {
            mqttClient.connect(mqttConnectOptions);
            mqttClient.subscribe(mqttSubscribeTopic);//订阅来自stm32客户端的消息
        }catch (Exception e){
            e.printStackTrace();
        }
    }
}
