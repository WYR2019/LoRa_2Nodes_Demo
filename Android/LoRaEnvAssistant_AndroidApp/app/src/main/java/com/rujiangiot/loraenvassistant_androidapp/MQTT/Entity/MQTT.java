package com.rujiangiot.loraenvassistant_androidapp.MQTT.Entity;

/**
 * @ClassName MQTTBean
 * @Auther name
 * @Date 2025/5/16
 * @Description
 */
public class MQTT {
    public String mqttDeviceId;
    public String mqttDeviceMode;
    public long mqttTimestamp;
    public boolean mqttIsNode1Connected;
    public boolean mqttIsNode2Connected;
    public String mqttTemp;
    public String mqttHumi;
    public String mqttLight;
    public String mqttFan;
    public String mqttHumidifier;
    public String mqttSmoke;
    public String mqttBeep;
    public String mqttWindows;
    public String mqttSmokeSafety;
    public String mqttFireSafety;

    public MQTT() {
    }
}
