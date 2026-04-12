package com.example.emqx_smartbadmintongym.Manager.Fragments;

import android.annotation.SuppressLint;
import android.graphics.Color;
import android.os.Bundle;
import androidx.fragment.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Switch;
import android.widget.TextView;
import com.example.emqx_smartbadmintongym.MQTT.MQTTHelper;
import com.example.emqx_smartbadmintongym.R;

public class ManagerNodesFragment extends Fragment implements View.OnClickListener, MQTTHelper.OnDataReceivedListener {
    private final String publishFanOnMsg = "{\"cmd\":{\"fan\":\"on\"}}";
    private final String publishFanOffMsg = "{\"cmd\":{\"fan\":\"off\"}}";
    private final String publishHumidifierOnMsg = "{\"cmd\":{\"humidifier\":\"on\"}}";
    private final String publishHumidifierOffMsg = "{\"cmd\":{\"humidifier\":\"off\"}}";
    private TextView tvEMQXConnectionStatus;
    private TextView tvTemp, tvHumi;
    private Switch swFanCtl, swHumidifierCtl;
    // 分别记录两个开关的状态
    private boolean fanState = false;
    private boolean humidifierState = false;

    public MQTTHelper mqttHelper = new MQTTHelper();

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View view = inflater.inflate(R.layout.fragment_manager_nodes, container, false);
        Init(view);

        // 设置数据接收监听器
        mqttHelper.setOnDataReceivedListener(this);

        // 在子线程中进行MQTT连接，避免阻塞UI
        new Thread(() -> {
            final int result = mqttHelper.MQTT_Init();
            // 更新UI需要在主线程进行
            if (getActivity() != null) {
                getActivity().runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        if (result == mqttHelper.CONNECT_SUCCESS_IDLE) {
                            tvEMQXConnectionStatus.setText("已连接我的球馆");
                            tvEMQXConnectionStatus.setTextColor(Color.GREEN);
                            mqttHelper.MQTT_Node1_Subscribe();
                        } else {
                            tvEMQXConnectionStatus.setText("连接失败");
                            tvEMQXConnectionStatus.setTextColor(Color.RED);
                        }
                    }
                });
            }
        }).start();

        return view;
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        // 移除监听器，避免内存泄漏
        mqttHelper.setOnDataReceivedListener(null);
        // 断开MQTT连接
        mqttHelper.disconnect();
    }

    @Override
    public void onClick(View v) {
        if (v.getId() == R.id.sw_fanctl) {
            fanState = !fanState;
            // 更新Switch的显示状态
            swFanCtl.setChecked(fanState);
            if (fanState) {
                mqttHelper.MQTT_Publish(mqttHelper.mqttPublishTopic, publishFanOnMsg);
            } else {
                mqttHelper.MQTT_Publish(mqttHelper.mqttPublishTopic, publishFanOffMsg);
            }
        } else if (v.getId() == R.id.sw_humidifierctl) {
            humidifierState = !humidifierState;
            swHumidifierCtl.setChecked(humidifierState);
            if (humidifierState) {
                mqttHelper.MQTT_Publish(mqttHelper.mqttPublishTopic, publishHumidifierOnMsg);
            } else {
                mqttHelper.MQTT_Publish(mqttHelper.mqttPublishTopic, publishHumidifierOffMsg);
            }
        }
    }

    // 实现MQTT数据接收回调接口
    @Override
    public void onTemperatureReceived(String temperature) {
        updateTemperatureUI(temperature);
    }

    @Override
    public void onHumidityReceived(String humidity) {
        updateHumidityUI(humidity);
    }

    private void updateTemperatureUI(final String temp) {
        // 确保Fragment处于活动状态
        if (getActivity() == null || !isAdded()) {
            return;
        }
        getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (tvTemp != null) {
                    tvTemp.setText(temp.equals("--") ? temp : temp + "℃");
                }
            }
        });
    }

    private void updateHumidityUI(final String humi) {
        if (getActivity() == null || !isAdded()) {
            return;
        }
        getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (tvHumi != null) {
                    tvHumi.setText(humi.equals("--") ? humi : humi + "%");
                }
            }
        });
    }

    private void Init(View view) {
        tvEMQXConnectionStatus = view.findViewById(R.id.tv_emqx_connection_status);
        tvTemp = view.findViewById(R.id.tv_temp);
        tvHumi = view.findViewById(R.id.tv_humi);
        swFanCtl = view.findViewById(R.id.sw_fanctl);
        swHumidifierCtl = view.findViewById(R.id.sw_humidifierctl);
        swFanCtl.setOnClickListener(this);
        swHumidifierCtl.setOnClickListener(this);
        // 初始化开关状态为关
        swFanCtl.setChecked(false);
        swHumidifierCtl.setChecked(false);
    }
}