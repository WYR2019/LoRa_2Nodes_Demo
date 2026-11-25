package com.rujiangiot.loraenvassistant_androidapp.SQLite.Device.Entity;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

public class Device {
    private final String deviceId;
    private final String deviceNickname;
    private final String deviceMode;
    private final long timestamp;

    public Device(@NonNull String deviceId, @Nullable String deviceNickname, @NonNull String deviceMode, long timestamp) {
        this.deviceId = deviceId;
        this.deviceNickname = deviceNickname;
        this.deviceMode = deviceMode;
        this.timestamp = timestamp;
    }

    public String getDeviceId() {
        return deviceId;
    }

    public String getDeviceNickname() {
        return deviceNickname;
    }

    public String getDeviceMode() {
        return deviceMode;
    }

    public long getTimestamp() {
        return timestamp;
    }

    @NonNull
    @Override
    public String toString() {
        return "Device{" +
                "deviceId='" + deviceId + '\'' +
                ", deviceNickname='" + deviceNickname + '\'' +
                ", deviceMode='" + deviceMode + '\'' +
                ", timestamp=" + timestamp +
                '}';
    }
}
