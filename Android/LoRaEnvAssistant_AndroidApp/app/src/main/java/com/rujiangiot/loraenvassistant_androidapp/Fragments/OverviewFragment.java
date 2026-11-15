package com.rujiangiot.loraenvassistant_androidapp.Fragments;

import android.annotation.SuppressLint;
import android.graphics.Color;
import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.appcompat.widget.SwitchCompat;
import androidx.fragment.app.Fragment;

import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.google.android.material.slider.RangeSlider;
import com.rujiangiot.loraenvassistant_androidapp.MQTT.Interface.OnSubscribedListener;
import com.rujiangiot.loraenvassistant_androidapp.MQTT.MQTTHelper;
import com.rujiangiot.loraenvassistant_androidapp.R;
import com.rujiangiot.loraenvassistant_androidapp.SQLite.Device.DeviceSQLiteHelper;

import java.util.List;

public class OverviewFragment extends Fragment implements View.OnClickListener, OnSubscribedListener {
    private final String PUBLISH_FAN_ON = "{\"cmd\":{\"fan\":\"on\"},\"timestamp\":\"%s\"}";
    private final String PUBLISH_FAN_OFF = "{\"cmd\":{\"fan\":\"off\"},\"timestamp\":\"%s\"}";
    private final String PUBLISH_HUMIDIFIER_ON = "{\"cmd\":{\"humidifier\":\"on\"},\"timestamp\":\"%s\"}";
    private final String PUBLISH_HUMIDIFIER_OFF = "{\"cmd\":{\"humidifier\":\"off\"},\"timestamp\":\"%s\"}";
    private final String PUBLISH_TEMPERATURE_MIN = "{\"cmd\":{\"temp_min\":\"%s\"},\"timestamp\":\"%s\"}";
    private final String PUBLISH_TEMPERATURE_MAX = "{\"cmd\":{\"temp_max\":\"%s\"},\"timestamp\":\"%s\"}";
    private final String PUBLISH_HUMIDITY_MIN = "{\"cmd\":{\"humi_min\":\"%s\"},\"timestamp\":\"%s\"}";
    private final String PUBLISH_HUMIDITY_MAX = "{\"cmd\":{\"humi_max\":\"%s\"},\"timestamp\":\"%s\"}";
    private boolean fanState = false, humidifierState = false, autoControlState = false;
    private TextView tvMqttConnectionStatus;
    private TextView tvTemp, tvHumi;
    /* 分别记录两个开关的状态 */
    private SwitchCompat swFanControl, swHumidifierControl, swAutoControl;
    private RangeSlider rsTemperature, rsHumidity;
    private MQTTHelper mqttHelper;
    private DeviceSQLiteHelper deviceSQLiteHelper;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View view = inflater.inflate(R.layout.fragment_overview, container, false);
        init(view);

        mqttHelper = new MQTTHelper();
        mqttHelper.setOnDataReceivedListener(this);
        deviceSQLiteHelper = new DeviceSQLiteHelper(getContext());

        /* 在子线程中进行MQTT连接，避免阻塞UI */
        new Thread(() -> {
            final MQTTHelper.connectStatus result = mqttHelper.mqttInit();
            /* 更新UI需要在主线程进行 */
            if (getActivity() != null) {
                getActivity().runOnUiThread(() -> {
                    if (result == MQTTHelper.connectStatus.CONNECT_SUCCESS) {
                        tvMqttConnectionStatus.setText("已连接至云平台");
                        tvMqttConnectionStatus.setTextColor(Color.GREEN);
                        mqttHelper.mqttGetSubscribeData();
                    } else {
                        tvMqttConnectionStatus.setText("连接失败");
                        tvMqttConnectionStatus.setTextColor(Color.RED);
                    }
                });
            }
        }).start();
        rsTemperature.addOnSliderTouchListener(new RangeSlider.OnSliderTouchListener() {
            @Override
            public void onStartTrackingTouch(@NonNull RangeSlider rangeSlider) {

            }

            @Override
            public void onStopTrackingTouch(@NonNull RangeSlider rangeSlider) {
                List<Float> values = rangeSlider.getValues();
                float tempMin = values.get(0);
                float tempMax = values.get(1);
                Log.d("RangeSlider", "设置的最低温度为：" + tempMin + "，" + "设置的最高温度为：" + tempMax);
                if (autoControlState) {
                    String msgTempMin = String.format(PUBLISH_TEMPERATURE_MIN, tempMin, System.currentTimeMillis());
                    String msgTempMax = String.format(PUBLISH_TEMPERATURE_MAX, tempMax, System.currentTimeMillis());
                    mqttHelper.mqttPublish(mqttHelper.MQTT_PUBLISH_TOPIC, msgTempMin);
                    mqttHelper.mqttPublish(mqttHelper.MQTT_PUBLISH_TOPIC, msgTempMax);
                }
            }
        });
        rsHumidity.addOnSliderTouchListener(new RangeSlider.OnSliderTouchListener() {
            @Override
            public void onStartTrackingTouch(@NonNull RangeSlider rangeSlider) {

            }

            @Override
            public void onStopTrackingTouch(@NonNull RangeSlider rangeSlider) {
                List<Float> values = rangeSlider.getValues();
                float humiMin = values.get(0);
                float humiMax = values.get(1);
                Log.d("RangeSlider", "设置的最低湿度为：" + humiMin + "，" + "设置的最高湿度为：" + humiMax);
                if (autoControlState) {
                    String msgHumiMin = String.format(PUBLISH_HUMIDITY_MIN, humiMin, System.currentTimeMillis());
                    String msgHumiMax = String.format(PUBLISH_HUMIDITY_MAX, humiMax, System.currentTimeMillis());
                    mqttHelper.mqttPublish(mqttHelper.MQTT_PUBLISH_TOPIC, msgHumiMin);
                    mqttHelper.mqttPublish(mqttHelper.MQTT_PUBLISH_TOPIC, msgHumiMax);
                }
            }
        });
        return view;
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        mqttHelper.setOnDataReceivedListener(null);                                                                 // 移除监听器，避免内存泄漏
        mqttHelper.mqttDisconnect();                                                                                // 断开MQTT连接
    }

    @Override
    public void onClick(View v) {
        String timestamp;
        if (v.getId() == R.id.sw_fanctl) {
            fanState = !fanState;
            swFanControl.setChecked(fanState);                                                                      // 更新Switch的显示状态
            if (fanState) {
                timestamp = String.format(PUBLISH_FAN_ON, System.currentTimeMillis());
            } else {
                timestamp = String.format(PUBLISH_FAN_OFF, System.currentTimeMillis());
            }
            mqttHelper.mqttPublish(mqttHelper.MQTT_PUBLISH_TOPIC, timestamp);                                   // 发布
        } else if (v.getId() == R.id.sw_humidifierctl) {
            humidifierState = !humidifierState;
            swHumidifierControl.setChecked(humidifierState);
            if (humidifierState) {
                timestamp = String.format(PUBLISH_HUMIDIFIER_ON, System.currentTimeMillis());
            } else {
                timestamp = String.format(PUBLISH_HUMIDIFIER_OFF, System.currentTimeMillis());
            }
            mqttHelper.mqttPublish(mqttHelper.MQTT_PUBLISH_TOPIC, timestamp);
        } else if (v.getId() == R.id.sw_autocontrol) {
            autoControlState = !autoControlState;
            swAutoControl.setChecked(autoControlState);
        }
    }

    @SuppressLint("WrongViewCast")
    private void init(View view) {
        tvMqttConnectionStatus = view.findViewById(R.id.tv_mqtt_connection_status);
        tvTemp = view.findViewById(R.id.tv_temp);
        tvHumi = view.findViewById(R.id.tv_humi);
        swFanControl = view.findViewById(R.id.sw_fanctl);
        swHumidifierControl = view.findViewById(R.id.sw_humidifierctl);
        swAutoControl = view.findViewById(R.id.sw_autocontrol);
        rsTemperature = view.findViewById(R.id.rs_temperature);
        rsHumidity = view.findViewById(R.id.rs_humidity);
        swFanControl.setOnClickListener(this);
        swHumidifierControl.setOnClickListener(this);
        swAutoControl.setOnClickListener(this);
        /* 初始化开关状态为关 */
        swFanControl.setChecked(false);
        swHumidifierControl.setChecked(false);
        swAutoControl.setChecked(false);
    }

    @Override
    public void onDeviceConnected(String deviceId, String deviceMode, long timestamp) {
        if (!deviceSQLiteHelper.isDeviceExist(deviceId)) {
            deviceSQLiteHelper.insertDeviceData(deviceId, deviceMode, timestamp);
        } else {
            deviceSQLiteHelper.updateDeviceTimestamp(deviceId, timestamp);
        }
    }

    @Override
    public void onNodesConnected(boolean isNodesConnected) {

    }

    /**
     * 实现MQTT数据接收回调接口
     *
     * @param temperature 温度
     */
    @Override
    public void onTemperatureReceived(String temperature) {
        /* 确保Fragment处于活动状态 */
        if (getActivity() == null || !isAdded())
            return;
        getActivity().runOnUiThread(() -> {
            if (tvTemp != null)
                tvTemp.setText(temperature.equals("--") ? temperature : temperature + "℃");
        });
    }

    @Override
    public void onHumidityReceived(String humidity) {
        if (getActivity() == null || !isAdded())
            return;
        getActivity().runOnUiThread(() -> {
            if (tvHumi != null)
                tvHumi.setText(humidity.equals("--") ? humidity : humidity + "%");
        });
    }
}