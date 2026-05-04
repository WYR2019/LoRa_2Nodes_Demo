package com.example.emqx_smartbadmintongym.MQTT.Bean;

/**
 * @ClassName MQTTBean
 * @Auther name
 * @Date 2025/5/16
 * @Description
 */
public class MQTTBean {

    private String jsonTemp;
    private String jsonHumi;
    private String jsonSmoke;
    private String jsonFire;
    private String jsonBeep;
    private String jsonServo;
    private String jsonLight;

//    public MQTTBean(String jsonTemp, String jsonHumi, String jsonSmoke, String jsonFire, String jsonBeep, String jsonServo, String jsonLight) {
//        this.jsonTemp = jsonTemp;
//        this.jsonHumi = jsonHumi;
//        this.jsonSmoke = jsonSmoke;
//        this.jsonFire = jsonFire;
//        this.jsonBeep = jsonBeep;
//        this.jsonServo = jsonServo;
//        this.jsonLight = jsonLight;
//    }

    public String getJsonTemp() {
        return jsonTemp;
    }

    public void setJsonTemp(String jsonTemp) {
        this.jsonTemp = jsonTemp;
    }

    public String getJsonHumi() {
        return jsonHumi;
    }

    public void setJsonHumi(String jsonHumi) {
        this.jsonHumi = jsonHumi;
    }

    public String getJsonSmoke() {
        return jsonSmoke;
    }

    public void setJsonSmoke(String jsonSmoke) {
        this.jsonSmoke = jsonSmoke;
    }

    public String getJsonFire() {
        return jsonFire;
    }

    public void setJsonFire(String jsonFire) {
        this.jsonFire = jsonFire;
    }

    public String getJsonBeep() {
        return jsonBeep;
    }

    public void setJsonBeep(String jsonBeep) {
        this.jsonBeep = jsonBeep;
    }

    public String getJsonServo() {
        return jsonServo;
    }

    public void setJsonServo(String jsonServo) {
        this.jsonServo = jsonServo;
    }

    public String getJsonLight() {
        return jsonLight;
    }

    public void setJsonLight(String jsonLight) {
        this.jsonLight = jsonLight;
    }

    @Override
    public String toString() {
        return "MQTTBean{" +
                "jsonTemp='" + jsonTemp + '\'' +
                ", jsonHumi='" + jsonHumi + '\'' +
                ", jsonSmoke='" + jsonSmoke + '\'' +
                ", jsonFire='" + jsonFire + '\'' +
                ", jsonBeep='" + jsonBeep + '\'' +
                ", jsonServo='" + jsonServo + '\'' +
                ", jsonLight='" + jsonLight + '\'' +
                '}';
    }
}
