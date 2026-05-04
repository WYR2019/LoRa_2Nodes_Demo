package com.example.emqx_smartbadmintongym.Manager.Fragments;

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

public class ManagerGymManFragment extends Fragment implements View.OnClickListener{

    private TextView tvSmoke, tvFire, tvBeep, tvServo, tvLight;
    private Switch swLED1Ctl, swLED2Ctl, swLED3Ctl;
    private int clickFlag = 0;
    MQTTHelper mqttHelper = new MQTTHelper();

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View view =  inflater.inflate(R.layout.fragment_manager_gymmanagement, container, false);
        Init(view);
//        EmqxAppUiThread();
        return view;
    }

    @Override
    public void onClick(View v) {
        if (v.getId() == R.id.sw_led1ctl) {
            if (clickFlag == 0) {
                mqttHelper.MQTT_Publish(mqttHelper.mqttPublishTopic,"");
                clickFlag = 1;
            }
            else {
                mqttHelper.MQTT_Publish(mqttHelper.mqttPublishTopic,"");
                clickFlag = 0;
            }
        } else if (v.getId() == R.id.sw_led2ctl) {
            if (clickFlag == 0) {
                mqttHelper.MQTT_Publish(mqttHelper.mqttPublishTopic,"{\\\"cmd\\\":{\\\"led\\\":\\\"on\\\"}}");
                clickFlag = 1;
            }
            else {
                mqttHelper.MQTT_Publish(mqttHelper.mqttPublishTopic,"{\\\"cmd\\\":{\\\"led\\\":\\\"off\\\"}}");
                clickFlag = 0;
            }
        } else if (v.getId() == R.id.sw_led3ctl) {
            if (clickFlag == 0) {
                mqttHelper.MQTT_Publish(mqttHelper.mqttPublishTopic,"@LED3_ON");
                clickFlag = 1;
            }
            else {
                mqttHelper.MQTT_Publish(mqttHelper.mqttPublishTopic,"@LED3_OFF");
                clickFlag = 0;
            }
        }
    }

    private void EmqxAppUiThread() {
        getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (mqttHelper.jsonSmoke.equals("0")) {
                    tvSmoke.setText("安全");
                    tvSmoke.setTextColor(Color.GREEN);
                } else if (mqttHelper.jsonSmoke.equals("1")){
                    tvSmoke.setText("危险");
                    tvSmoke.setTextColor(Color.RED);
                } if (mqttHelper.jsonFire.equals("0")) {
                    tvFire.setText("安全");
                    tvFire.setTextColor(Color.GREEN);
                } else if (mqttHelper.jsonFire.equals("1")) {
                    tvFire.setText("危险");
                    tvFire.setTextColor(Color.RED);
                } if (mqttHelper.jsonBeep.equals("0")) {
                    tvBeep.setText("安静");
                    tvBeep.setTextColor(Color.GREEN);
                } else if (mqttHelper.jsonBeep.equals("1")) {
                    tvBeep.setText("响");
                    tvBeep.setTextColor(Color.RED);
                } if (mqttHelper.jsonServo.equals("0")) {
                    tvServo.setText("关");
                } else if (mqttHelper.jsonServo.equals("1")) {
                    tvServo.setText("开");
                } if (mqttHelper.jsonLight.equals("0")) {
                    tvLight.setText("暗");
                } else if (mqttHelper.jsonLight.equals("1")) {
                    tvLight.setText("亮");
                }
            }
        });
    }

    private void Init(View view) {
        tvSmoke = view.findViewById(R.id.tv_smoke);
        tvFire = view.findViewById(R.id.tv_fire);
        tvBeep = view.findViewById(R.id.tv_beep);
        tvServo = view.findViewById(R.id.tv_servo);
        tvLight = view.findViewById(R.id.tv_brightness);
        swLED1Ctl = view.findViewById(R.id.sw_led1ctl);
        swLED2Ctl = view.findViewById(R.id.sw_led2ctl);
        swLED3Ctl = view.findViewById(R.id.sw_led3ctl);
        swLED1Ctl.setOnClickListener(this);
        swLED2Ctl.setOnClickListener(this);
        swLED3Ctl.setOnClickListener(this);
    }
}