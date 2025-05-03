package com.example.emqx_smartbadmintongym.Manager.Fragments;

import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;

import androidx.fragment.app.Fragment;

import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import com.example.emqx_smartbadmintongym.Manager.Nodes.ManagerSafetyMonitoringNodeActivity;
import com.example.emqx_smartbadmintongym.Manager.Nodes.ManagerTemperatureNodeActivity;
import com.example.emqx_smartbadmintongym.R;

import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;
import org.json.JSONException;
import org.json.JSONObject;

public class ManagerNodesFragment extends Fragment implements View.OnClickListener {

    private final String serverUri = "tcp://36.111.167.90:1883";
    private final String clientId = "EMQXSmartBadmintonGym&Android";
    private final String userName = "Android";//不能重名
    private final String passWord = "IPSK25em.";
    private final String mqttSubscribeTopic = "EMQXSmartBadmintonGym/devices/stm32/topics";//接收来自云平台的消息
    private final String mqttPublishTopic = "EMQXSmartBadmintonGym/devices/Android/topics";

    public MqttClient mqttClient;
    private MqttConnectOptions mqttConnectOptions;

    private TextView tvEMQXConnectionStatus;
    private TextView tvTemp,tvHumi;

    private Button btnNode1;
    private Button btnNode2;
    private Button btnNode3;
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View view = inflater.inflate(R.layout.fragment_manager_nodes, container, false);
        Init(view);
        MQTT_Init();
        mqttClient.setCallback(new MqttCallback() {
            @Override
            public void connectionLost(Throwable cause) {
//                Toast.makeText(ManagerTemperatureNodeActivity.this, "连接断开，请检查网络", Toast.LENGTH_SHORT).show();
            }

            @Override
            public void messageArrived(String topic, MqttMessage message) throws Exception {
                String msg = new String(message.getPayload());
                Log.v("订阅数据",msg);
                getActivity().runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        try {
                            JSONObject jsonObject = new JSONObject(msg);
                            String jsonTemp = jsonObject.getString("Temperature");
                            String jsonHumi = jsonObject.getString("Humidity");
                            Log.v("温度", jsonTemp);
                            Log.v("湿度", jsonHumi);
                            tvTemp.setText(jsonTemp + "℃");
                            tvHumi.setText(jsonHumi + "%");
                        }catch (JSONException e) {
                            e.printStackTrace();
                        }
                    }
                });
            }

            @Override
            public void deliveryComplete(IMqttDeliveryToken token) {

            }
        });
        return view;
    }

    @Override
    public void onClick(View v) {
        if (v.getId()==R.id.btn_node1){
            Intent intent = new Intent(getActivity(), ManagerTemperatureNodeActivity.class);
            startActivity(intent);
            onDestroy();
        }else if (v.getId()==R.id.btn_node2){
            Intent intent = new Intent(getActivity(), ManagerSafetyMonitoringNodeActivity.class);
            startActivity(intent);
        }
    }

    public void MQTT_Init() {
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
            Toast.makeText(getActivity(),"连接失败",Toast.LENGTH_LONG).show();
            tvEMQXConnectionStatus.setText("连接失败");
        }
    }

    public void MQTT_Connect(){
        try {
            mqttClient.connect(mqttConnectOptions);
            if (mqttClient.isConnected())
            {
                Toast.makeText(getActivity(),"连接成功了",Toast.LENGTH_LONG).show();
                tvEMQXConnectionStatus.setText("连接成功");
                tvEMQXConnectionStatus.setTextColor(Color.GREEN);
            }else
            {
                Toast.makeText(getActivity(), "连接失败", Toast.LENGTH_SHORT).show();
                tvEMQXConnectionStatus.setText("连接失败");
            }
            mqttClient.subscribe(mqttSubscribeTopic);//订阅来自stm32客户端的消息
        }catch (Exception e){
            e.printStackTrace();
            Toast.makeText(getActivity(), "连接失败", Toast.LENGTH_SHORT).show();
            tvEMQXConnectionStatus.setText("连接失败");
        }
    }

    private void Init(View view) {
        btnNode1 = view.findViewById(R.id.btn_node1);
        btnNode2 = view.findViewById(R.id.btn_node2);
        btnNode3 = view.findViewById(R.id.btn_node3);
        tvEMQXConnectionStatus = view.findViewById(R.id.tv_emqx_connection_status);
        tvTemp = view.findViewById(R.id.tv_temp);
        tvHumi = view.findViewById(R.id.tv_humi);
        btnNode1.setOnClickListener(this);
        btnNode2.setOnClickListener(this);
        btnNode3.setOnClickListener(this);
    }
}