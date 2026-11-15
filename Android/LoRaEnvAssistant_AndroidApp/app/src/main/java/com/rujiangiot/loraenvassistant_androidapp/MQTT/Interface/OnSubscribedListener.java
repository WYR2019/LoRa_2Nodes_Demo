package com.rujiangiot.loraenvassistant_androidapp.MQTT.Interface;

public interface OnSubscribedListener {
    void onDeviceConnected(String deviceId, String deviceMode, long timestamp);
    void onNodesConnected(boolean isNodesConnected);
    void onTemperatureReceived(String temperature);
    void onHumidityReceived(String humidity);
}
