package com.rujiangiot.loraenvassistant_androidapp.Tools.SharedPreferences.User;

import android.content.Context;
import android.content.SharedPreferences;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

public class UserSPHelper {
    private static final String SP_NAME = "user";
    private static final String SP_ITEM_TEL_NUMBER = "tel_number";
    private static final String SP_ITEM_USERNAME = "username";
    /* 登录过期时间，以毫秒为单位，设置3天 */
    private static final long LOGIN_EXPIRATION_TIME = 3L * 24 * 60 * 60 * 1000;
    private final SharedPreferences sharedPreferences;
    private final SharedPreferences.Editor editor;

    public UserSPHelper(@NonNull Context context) {
        sharedPreferences = context.getSharedPreferences(SP_NAME, Context.MODE_PRIVATE);
        editor = sharedPreferences.edit();
    }

    public void spAutoInput(@Nullable String telNumber, @Nullable String username) {
        if (telNumber != null) {
            editor.putString(SP_ITEM_TEL_NUMBER, telNumber);
        }
        if (username != null) {
            editor.putString(SP_ITEM_USERNAME, username);
        }
        editor.apply();
    }

    public String spGetUsername() {
        return sharedPreferences.getString(SP_ITEM_USERNAME, "");
    }

    public String spGetTelNumber() {
        return sharedPreferences.getString(SP_ITEM_TEL_NUMBER, "");
    }

    public void spSaveLoginTime() {
        long currentTime = System.currentTimeMillis();
        sharedPreferences.edit().putLong("last_login_time", currentTime).apply();
    }

    public Long spGetLastLoginTime() {
        return sharedPreferences.getLong("last_login_time", 0);
    }

    public boolean spGetLoginStatus() {
        long lastLoginTime = spGetLastLoginTime();
        if (lastLoginTime == -1)
            return true;
        long currentTime = System.currentTimeMillis();
//        return (currentTime - lastLoginTime) > 120000; // 2分钟测试
        return (currentTime - lastLoginTime) > LOGIN_EXPIRATION_TIME;
    }

    public void spLogout() {
        editor.clear();
        editor.apply();
    }
}