package com.rujiangiot.loraenvassistant_androidapp.Tools.GetAppVersion;

import android.content.Context;
import android.content.pm.PackageManager;

public class GetAppVersion {
    public static int getAppVersionCode(Context context) throws PackageManager.NameNotFoundException {
        int versionCode = 0;
        try {
            versionCode = context.getPackageManager().getPackageInfo(context.getPackageName(), 0).versionCode;
        } catch (PackageManager.NameNotFoundException e) {
            throw new RuntimeException(e);
        }
        return versionCode;
    }

    public static String getAppVersionName(Context context) {
        String versionName = "";
        try {
            versionName = context.getPackageManager().
                    getPackageInfo(context.getPackageName(), 0).versionName;
        } catch (PackageManager.NameNotFoundException e) {
            throw new RuntimeException(e);
        }
        return versionName;
    }
}
