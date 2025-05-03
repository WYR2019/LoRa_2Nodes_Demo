package com.example.emqx_smartbadmintongym.Manager.Nodes;

import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;

import com.example.emqx_smartbadmintongym.R;
import com.example.emqx_smartbadmintongym.Manager.Fragments.ManagerNodesFragment;

import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;
import org.json.JSONException;
import org.json.JSONObject;

public class ManagerTemperatureNodeActivity extends AppCompatActivity {
    public MqttClient mqttClient;
    private MqttConnectOptions mqttConnectOptions;
    private ManagerNodesFragment managerNodesFragment;
    private TextView tvTemp,tvHumi;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_manager_temperature_node);
//        Init();

    }

    private void Init() {
        tvTemp = findViewById(R.id.tv_temp);
        tvHumi = findViewById(R.id.tv_humi);
    }
}